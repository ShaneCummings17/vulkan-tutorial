#include <vulkan-tutorial/devices/Device.hpp>

// Standard C++ Libraries
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <limits>
#include <ranges>

// Required device extensions
inline const std::vector<const char*> requiredDeviceExtensions = {
    vk::KHRSwapchainExtensionName
};

// Local variables
namespace {
    constexpr bool PREFER_DEDICATED_GPU = true;
}

// Local helper functions
namespace {
    // Test each device to confirm suitability
    bool isDeviceSuitable(vk::raii::PhysicalDevice const &physicalDevice) {

        // Avoid duplicate function calls; saves a CPU cycle. Doesn't really matter but eh
        auto physicalDeviceProperties = physicalDevice.getProperties();

        // STEP #1: Does the device support Vulkan 13 and up?
        bool supportsVulkan1_3 = physicalDeviceProperties.apiVersion >= vk::ApiVersion13;

        // STEP #2: Does the device support graphics queue commands?
        auto queueFamilies = physicalDevice.getQueueFamilyProperties();
        bool supportsGraphics = std::ranges::any_of(
            queueFamilies, 
            [](auto const &qfp) {
                return static_cast<bool>(qfp.queueFlags & vk::QueueFlagBits::eGraphics); // Bitwise check that the queueFlags matches the eGraphics bits for any of the queue families
            }
        );

        // STEP #3: Does the device support all of our required device extensions?
        // Note; DEVICE extensions and INSTANCE extensions are different. that's why we have a separate function for checking instance extensions, and are checking device extensions here.
        auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
        bool supportsAllRequiredExtensions = std::ranges::all_of(
            requiredDeviceExtensions,
            [&availableDeviceExtensions](auto const &requiredDeviceExtension) {
                return std::ranges::any_of(
                    availableDeviceExtensions,
                    [requiredDeviceExtension](auto const &availableDeviceExtension) {
                        return strcmp(availableDeviceExtension.extensionName, requiredDeviceExtension) == 0;
                    }
                );
            }
        );

        // STEP #4: Does the device support all of our required features?
        auto features = physicalDevice.template getFeatures2<
            vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan11Features,
            vk::PhysicalDeviceVulkan13Features,
            vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
        >();
        bool supportsRequiredFeatures =
            features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
            features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
            features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;
        
        // STEP #5 (Custom check not in tutorial) Check that device is a dedicated GPU
        // Note; this normally isn't a smart binary check, because some computers don't have dedicated GPU hardware. This limits
        // how many machines this code can run on. HOWEVER; since this is a tutorial, I'm doing this to force a run against my machine's GPU
        bool meetsGPURequirement = true;
        if (PREFER_DEDICATED_GPU) { // Constant defined at the top for easy shutoff
            meetsGPURequirement = (physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu);
        }

        // STEP #6: Return the device's compatibility
        return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures && meetsGPURequirement;

    }
}



/***** CONSTRUCTOR AND DESTRUCTOR *****/
Device::Device(
    const vk::raii::Instance &instance,
    const vk::raii::SurfaceKHR& surface
) {
    pickPhysicalDevice(instance);
    createLogicalDevice(surface);
}



/***** PUBLIC METHODS *****/
const vk::raii::PhysicalDevice& Device::getPhysicalDevice() const {
    return physicalDevice;
}

const vk::raii::Device& Device::getLogicalDevice() const {
    return logicalDevice;
}

const vk::raii::Queue& Device::getGraphicsQueue() const {
    return graphicsQueue;
}

const uint32_t Device::getQueueIndex() const {
    return queueIndex;
}



/***** PRIVATE METHODS *****/
void Device::pickPhysicalDevice(const vk::raii::Instance &instance) {
    // Get all graphics cards on the system
    auto physicalDevices = instance.enumeratePhysicalDevices();
    if (physicalDevices.empty()) { // If nothing supports Vulkan, end the execution
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    // Check each graphics card and see if it is suitable. It may support Vulkan generally, but can it do everything we want it to?
    auto const devIter = std::ranges::find_if( // Iterate over every element in physicalDevices, and run isDeviceSuitable against it with a lambda function
        physicalDevices,
        [&](auto const &device) {
            return isDeviceSuitable(device);
        }
    );

    // Exit program if a suitable device is not found
    if (devIter == physicalDevices.end()) {
        throw std::runtime_error("failed to find a suitable GPU!");
    };

    // Otherwise, pick the the first suitable device found
    physicalDevice = *devIter;

    // Print the chosen device to stdout
    std::cout << "Selected Physical Device: " << physicalDevice.getProperties().deviceName << std::endl;
}

void Device::createLogicalDevice(const vk::raii::SurfaceKHR &surface) {

    // STEP #1: Get the physical device's queue family info
    std::vector<vk::QueueFamilyProperties> queueFamilyPropeties = physicalDevice.getQueueFamilyProperties();


    // STEP #2: Get the first index into queueFamilyProperties that supports both graphics and present
    // Graphics == execution of rendering commands
    // Present == displaying images onto window surface via swapchain
    // We could technically use different queues for graphics and present, but this is inefficient and should only be done in rare situations
    // Doing the loop a bit different here than the tutorial cause I have access to C++ 23 features
    for (auto [index, qfp] : std::views::enumerate(queueFamilyPropeties)) { // Loop over all the queues in the queue family
        if ((qfp.queueFlags & vk::QueueFlagBits::eGraphics) && physicalDevice.getSurfaceSupportKHR(index, *surface)) { // Return the first one that supports both graphics and present
            queueIndex = index;
            break;
        }
    }

    if (queueIndex == UINT32_MAX) { // Were none found?
        throw std::runtime_error("could not find a queue family for graphics and present --> terminating");
    }


    // STEP #3: Set the priority of the chosen queue (help the GPU decide which command in the buffer should run first)
    // Takes a float between 0.0 and 1.0
    // Doesn't really matter in this situation; setting to 0.5 for easy queue ordering later if we add a compute queue or something
    float queuePriority = 0.5f;



    // STEP #4: Create the info for the queue that will be passed to the Vulkan instance
    // Enable synchronization2 features
    vk::PhysicalDeviceSynchronization2Features synchronization2Features {
        .synchronization2 = vk::True
    };

    // Enable dynamic rendering features
    vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{
        .dynamicRendering = vk::True
    };

    // Chain the synchronization and dynamic rendering features structs together
    synchronization2Features.pNext = &dynamicRenderingFeatures;

    // Create info
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo {
        .queueFamilyIndex = queueIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };


    // STEP #5: Specify the set of device features that will be used (come back later)
    vk::PhysicalDeviceFeatures deviceFeatures;
    
    // STEP #6: Enable the device's access to Vulkan 13 features and lower using structure chaining (basically a linked list)
    // Only Vulkan 1.0 features are enabled by default for backwards compat w/older GPUs;
    // using newer features requires explicit opt-in
    // Extensions -- add new Vulkan API functionality
    // Validation Layers -- add hooks into Vulkan APIs to perform some action
    // Features -- GPU features to manually give the device access to
    vk::StructureChain<
        vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan11Features,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
    > featureChain = {
        {},                                 // vk::PhysicalDevicesFeatures2 (empty; come back later)
        {.shaderDrawParameters = true},     // Enable shader draw parameters from Vulkan 1.1
        {
            .synchronization2 = true,       // Enable synchronization2
            .dynamicRendering = true        // Enable dynamic rendering from Vulkan 1.3 (i.e., no VkFrameBuffer object)
        },         
        {.extendedDynamicState = true}      // Enable extended dynamic state from the extension; lets us change specific pipeline settings on the fly during execution
    };


    // STEP #7: Create the logical device and graphics queue
    vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(), // Provide a pointer to the first element of the structure chain; Vulkan is smart enough to traverse the structure chain itself
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size()),
        .ppEnabledExtensionNames = requiredDeviceExtensions.data()
    };

    logicalDevice = vk::raii::Device(physicalDevice, deviceCreateInfo);
    graphicsQueue = vk::raii::Queue(logicalDevice, queueIndex, 0);
}