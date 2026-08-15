#include <vulkan-tutorial/vulkan-api/Vulkan.hpp>

// Standard C++ Libraries
#include <iostream>
#include <algorithm>
#include <cstring>

// Local variables
namespace {
    // Conditional compilation
    #ifdef NDEBUG // Only enable validation layers if running in debug mode; production release has them disabled to remove unnecessary overhead
    constexpr bool enableValidationLayers = false;
    #else
    constexpr bool enableValidationLayers = true;
    #endif

    // List of validation layers
    const std::vector<char const*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
}

// Local helper functions
namespace {
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT severity, // Severity level of the message
        vk::DebugUtilsMessageTypeFlagsEXT type, // Type of debug message
        const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, // A pointer to the debug message
        void *pUserData // nullptr; not applicable for this implementation. would contain a pointer to custom data I provide
    ) {
        std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl; // Send the debug message over to the debug messenger

        return vk::False;
    }
}


/***** CONSTRUCTOR AND DESTRUCTOR *****/
Vulkan::Vulkan(const char *appName, const char *engine, const Window &window) :
    appName(appName),
    engine(engine),
    window(window)
{
    createInstance(); // Create Vulkan API instance
    setupDebugMessenger(); // Setup debug validation layers
    initSurface(); // Initialize the surface the Vulkan API will be rendering to

    // Pick the Physical Device
    physicalDevice = std::make_unique<PhysicalDevice>(instance);

    // Create the Logical Device
    logicalDevice = std::make_unique<LogicalDevice>(physicalDevice->getPhysicalDevice(), surface);

    // Create the Swapchain + Image views
    swapchain = std::make_unique<Swapchain>(physicalDevice->getPhysicalDevice(), surface, window, logicalDevice->getLogicalDevice());

    // Create the graphics pipeline
    graphicsPipeline = std::make_unique<GraphicsPipeline>(logicalDevice->getLogicalDevice(), swapchain->getSwapchainExtent(), swapchain->getSwapchainSurfaceFormat());

    // Create the command pool and buffer
    commands = std::make_unique<Commands>(logicalDevice->getLogicalDevice(), logicalDevice->getQueueIndex());

    // Create the sync objects
    syncObjects = std::make_unique<SyncObjects>(logicalDevice->getLogicalDevice(), commands->getCommandBuffer(0), swapchain->getSwapchain());
}

Vulkan::~Vulkan() {};



/***** PUBLIC METHODS *****/
// Expose the logical device
const LogicalDevice& Vulkan::getLogicalDeviceObject() const {
    return *logicalDevice;
}

// Expose the syncObjects
const SyncObjects& Vulkan::getSyncObjects() const {
    return *syncObjects;
}

// Expose the swapchain object
const Swapchain& Vulkan::getSwapchainObject() const {
    return *swapchain;
}

// Expose the commands object
const Commands& Vulkan::getCommandsObject() const {
    return *commands;
}

// Expose the graphics pipeline object
const GraphicsPipeline& Vulkan::getGraphicsPipelineObject() const {
    return *graphicsPipeline;
}



/***** PRIVATE METHODS *****/
// Create the Vulkan instance
void Vulkan::createInstance() {
    // Define info about the application (Name, App Version, Engine, Engine Version, Vulkan API Version)
    const vk::ApplicationInfo appInfo{
        .pApplicationName = appName,
        .applicationVersion = vk::makeVersion(1, 0, 0),
        .pEngineName = engine,
        .engineVersion = vk::makeVersion(1, 0, 0),
        .apiVersion = vk::ApiVersion14
    };

    // Get the required Validation Layers (i.e., components that hook into Vulkan function calls to perform additional operations);
    // If enabled, creates a copy of the validationLayers object
    // Validation Layers are generally used for debugging interaction between the GPU driver and the graphics application
    std::vector<char const*> requiredLayers;
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
            throw std::runtime_error("Required layer not supported" + std::string(*unsupportedLayerIt));
    };

    
    // Get the required Extensions (modular add-ons that expand the API; basically optional libraries)
    std::vector<const char*> requiredExtensions = getRequiredInstanceExtensions();

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
    
    // If literally any extension was found that is not available, throw a runtime error
    if (unsupportedPropertyIt != requiredExtensions.end()) { 
        throw std::runtime_error("Required extension not supported" + std::string(*unsupportedPropertyIt));
    };

    // Tells Vulkan driver which global extensions and validation layers we want to use (apply to entire program, not a specific device)
    vk::InstanceCreateInfo createInfo{
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data()
    };

    // Finally create the damn Vulkan instance; when it goes out of scope, the object will be deconstructed and free memory (RAII -- aka the point of the Vulkan HPP wrappers)
    instance = vk::raii::Instance(context, createInfo);
}

std::vector<const char*> Vulkan::getRequiredInstanceExtensions() { // Move required extensions into a separate helper function; extensions outside of glfw can be added to this helper function in future
    // Get the required global instance extensions from the window; basically tell Vulkan how to interface w/the window on the OS the binary is running on
    std::vector<const char *> extensions = window.getRequiredWindowExtensions();

    if (enableValidationLayers) { // If validation layers are enabled, include the debug messenger extension as required
        extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }

    return extensions;
}


// Create debug hooks
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


// Initialize surface
void Vulkan::initSurface() {
    // Grab the surface from the window object
    VkSurfaceKHR _surface = window.createSurface(*instance);

    // Wrap the raw Vulkan surface handle in a RAII C++ object so we don't gotta remember to delete it later
    // Death to memory leaks!
    surface = vk::raii::SurfaceKHR(instance, _surface);
}