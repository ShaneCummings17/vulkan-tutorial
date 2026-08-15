#include <vulkan-tutorial/devices/LogicalDevice.hpp>

// Standard C++ Libraries
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <ranges>

// Shared Variables
#include <vulkan-tutorial/devices/DeviceConfig.hpp>



/***** CONSTRUCTOR AND DESTRUCTOR *****/
LogicalDevice::LogicalDevice(const vk::raii::PhysicalDevice &physicalDevice, vk::raii::SurfaceKHR &surface) {
    createLogicalDevice(physicalDevice, surface);
}

LogicalDevice::~LogicalDevice() {}


/***** PUBLIC METHODS *****/
const vk::raii::Device& LogicalDevice::getLogicalDevice() const {
    return logicalDevice;
}

const vk::raii::Queue& LogicalDevice::getGraphicsQueue() const {
    return graphicsQueue;
}

const uint32_t LogicalDevice::getQueueIndex() const {
    return queueIndex;
}


/***** PRIVATE METHODS *****/
void LogicalDevice::createLogicalDevice(const vk::raii::PhysicalDevice &physicalDevice, const vk::raii::SurfaceKHR &surface) {

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