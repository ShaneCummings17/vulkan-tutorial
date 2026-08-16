#include <vulkan-tutorial/devices/PhysicalDevice.hpp>

// Standard C++ Libraries
#include <algorithm>
#include <stdexcept>
#include <iostream>

// Shared Variables
#include <vulkan-tutorial/devices/DeviceConfig.hpp>

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
PhysicalDevice::PhysicalDevice(const vk::raii::Instance &instance) {
    pickPhysicalDevice(instance);
}


/***** PUBLIC METHODS *****/
const vk::raii::PhysicalDevice& PhysicalDevice::getPhysicalDevice() const {
    return physicalDevice;
}


/***** PRIVATE METHODS *****/
void PhysicalDevice::pickPhysicalDevice(const vk::raii::Instance &instance) {
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