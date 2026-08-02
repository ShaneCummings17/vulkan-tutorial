#include <vulkan-tutorial/vulkan-api/Vulkan.hpp>

// Standard C++ Libraries
#include <iostream>
#include <vector>

// Local variables
namespace {
    #ifdef NDEBUG
        constexpr bool enableValidationLayers = false;
    #else
        constexpr bool enableValidationLayers = true;
    #endif

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
}

// Local functions
namespace {
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT severity, // Severity level of the message
        vk::DebugUtilsMessageTypeFlagsEXT type, // Type of debug message
        const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, // A pointer to the debug message
        void *pUserData // nullptr; not applicable for this implementation. would contain a pointer to custom data I provide
    )
    {
        std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl; // Send the debug message over to the debug messenger
        return vk::False;
    }
}

// Constructor
Vulkan::Vulkan(
    const char *appName,
    const char *engine, 
    const std::vector<const char*> requiredWindowExtensions,
    VkSurfaceKHR rawSurface
) {
    createInstance();
    setupDebugMessenger();
    surface = vk::raii::SurfaceKHR(instance, rawSurface);
};

// Destructor
Vulkan::~Vulkan() {};

// Get a pointer to the raw instance
VkInstance Vulkan::getRawInstance() const {
    return *instance; 
}

// Initialize Vulkan instance in memory
void Vulkan::createInstance() {
    // Define info about the application (Name, App Version, Engine, Engine Version, Vulkan API Version)
    const vk::ApplicationInfo appInfo{
        .pApplicationName = appName,
        .applicationVersion = vk::makeVersion(1, 0, 0),
        .pEngineName = engine,
        .engineVersion = vk::makeVersion(1, 0, 0),
        .apiVersion = vk::ApiVersion14
    };

    // Get the required Validation Layers
    std::vector<char const*> requiredValidationLayers = getRequiredValidationLayers();

    // Get the required Extensions
    std::vector<char const*> requiredExtensions = getRequiredExtensions();

     // Tells Vulkan driver which global extensions and validation layers we want to use (apply to entire program, not a specific device)
    vk::InstanceCreateInfo createInfo{
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size()),
        .ppEnabledLayerNames = requiredValidationLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data()
    };

    // Create the Vulkan instance
    instance = vk::raii::Instance(context, createInfo);
}

void Vulkan::setupDebugMessenger() {
    if (!enableValidationLayers) return; // Checks if we're in debug mode; if we're not, don't even bother setting this up

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags( // How severe is the message? Only output debug messages for warning or above. Can add eVerbose and eInfo to increase error output.
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );

    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags( // What type of message is it? Enabling all three for right now
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | 
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
    );

    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{ // Create the info block, including a pointer to the callback function to run with this info when a debug message event occurs
        .messageSeverity = severityFlags,
        .messageType = messageTypeFlags,
        .pfnUserCallback = &debugCallback
    };

    debugMessenger = instance.createDebugUtilsMessengerEXT( // Attach the debug messenger to read the output of the created vulkan instance
        debugUtilsMessengerCreateInfoEXT
    );
}

// ***HELPER FUNCTIONS***
// Get the required Validation Layers (i.e., components that hook into Vulkan function calls to perform additional operations);
// Validation Layers are generally used for debugging interaction between the GPU driver and the graphics application
std::vector<const char*> Vulkan::getRequiredValidationLayers() {
    std::vector<char const *> requiredLayers;

    // If we're not in debug mode, just return an empty vector
    if (enableValidationLayers) {
        requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    }

    // Check if the required validation layers are supported by the Vulkan implementation
    auto layerProperties = context.enumerateInstanceLayerProperties(); // Get all available global Vulkan layers on host system
    auto unsupportedLayerIt = std::ranges::find_if( // Search through requiredLayers for the first item that matches the condition; the condition being, does it find a layer that isn't present on the host system?
        requiredLayers,
        [&layerProperties](auto const &requiredLayer) {
            return std::ranges::none_of( // Is there no element in layerProperties that matches requiredLayer?
                layerProperties,
                [requiredLayer](auto const &layerProperty) {
                    return strcmp(layerProperty.layerName, requiredLayer) == 0;
                }
            );
        }
    );

    // If literally any layer was found that is not available, throw a runtime error
    if (unsupportedLayerIt != requiredLayers.end()) { 
        throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
    };

    // Return our list of requiredLayers
    return requiredLayers;

};

// Get the required Extensions (modular add-ons that expand the API; basically optional libraries)
std::vector<const char*> Vulkan::getRequiredExtensions() {
    // Get the required global instance extensions from the window
    std::vector<const char*> requiredExtensions = requiredWindowExtensions; // The unpacked list of all window extensions

    if (enableValidationLayers) { // If validation layers are enabled, include the debug messenger extension as required
        requiredExtensions.push_back(vk::EXTDebugUtilsExtensionName);
    }

    // Check if the required extensions are supported by the Vulkan implementation
    auto extensionProperties = context.enumerateInstanceExtensionProperties(); // Get all extensions supported by Vulkan; returns a struct
    auto unsupportedPropertyIt = std::ranges::find_if( // Search through requiredExtensions for the first item that matches the condition; the condition being, does it find a property that isn't present on the host system?
        requiredExtensions,
        [&extensionProperties](auto const &requiredExtension) {
            return std::ranges::none_of( // Is there no element in extensionProperties that matches requiredExtension?
                extensionProperties,
                [requiredExtension](auto const &extensionProperty) {
                    return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
                }
            );
        }
    );
    
    if (unsupportedPropertyIt != requiredExtensions.end()) {  // If literally any extension was found that is not available, throw a runtime error
        throw std::runtime_error("Required extension not supported" + std::string(*unsupportedPropertyIt));
    };

    return requiredExtensions;
};