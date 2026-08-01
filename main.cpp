#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_raii.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

// Constants
constexpr uint32_t WIDTH = 800; // Window width
constexpr uint32_t HEIGHT = 600; // Window height
constexpr bool PREFER_DEDICATED_GPU = true;

// Required Validation Layers
// aka hooks into vulkan function calls that add validation logic
const std::vector<char const*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

// Required Device Extensions
const std::vector<const char*> requiredDeviceExtensions = {
    vk::KHRSwapchainExtensionName
};


#ifdef NDEBUG // Only enable validation layers if running in debug mode; production release has them disabled to remove unnecessary overhead
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

// The meat and potatoes
class HelloTriangleApplication {
    public:
        void run() {
            initWindow();
            initVulkan();
            mainLoop();
            cleanup();
        }


    private:
        void initWindow() {
            glfwInit(); // Initialize glfw in memory
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Tell glfw to not create an OpenGL context
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Disable resizing of window

            window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr); // Initialize the window; fourth param specifies monitor, next param is OpenGL specific so we don't care

        }


        void initVulkan() {
            createInstance(); // Initialize Vulkan instance in memory. Allows devs to interact with the Vulkan API
            setupDebugMessenger(); // Set up the debug messages (if running in debug mode)
            pickPhysicalDevice(); // Pick the graphics card we're running on
        }

        void createInstance() {
             // Define info about the application (Name, App Version, Engine, Engine Version, Vulkan API Version)
            constexpr vk::ApplicationInfo appInfo{
                .pApplicationName = "Hello Triangle",
                .applicationVersion = vk::makeVersion(1, 0, 0),
                .pEngineName = "No Engine",
                .engineVersion = vk::makeVersion(1, 0, 0),
                .apiVersion = vk::ApiVersion14
            };

            // Get the required validation layers; if enabled, creates a copy of the validationLayers object
            std::vector<char const*> requiredLayers;
            if (enableValidationLayers) {
                requiredLayers.assign(validationLayers.begin(), validationLayers.end());
            }

            // Check if the required layers are supported by the Vulkan implementation
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

            if (unsupportedLayerIt != requiredLayers.end()) { // If literally any layer was found that is not available, throw a runtime error
                throw std::runtime_error("Required layer not supported!");
            };

            // Get the required extensions
            auto requiredExtensions = getRequiredInstanceExtensions();

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
            
            if (unsupportedPropertyIt != requiredExtensions.end()) {
                throw std::runtime_error("Required extension not supported" + std::string(*unsupportedPropertyIt));
            };
            
            vk::InstanceCreateInfo createInfo{ // Tells Vulkan driver which global extensions and validation layers we want to use (apply to entire program, not a specific device)
                .pApplicationInfo = &appInfo,
                .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
                .ppEnabledLayerNames = requiredLayers.data(),
                .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
                .ppEnabledExtensionNames = requiredExtensions.data()
            };


            // Finally create the damn Vulkan instance; when it goes out of scope, the object will be deconstructed and free memory (RAII -- aka the point of C++/Vulkan HPP)
            instance = vk::raii::Instance(context, createInfo);

        }

        std::vector<const char*> getRequiredInstanceExtensions() { // Move required extensions into a separate helper function; extensions outside of glfw can be added to this helper function in future
            // Get the required global instance extensions from GLFW; basically tell Vulkan how to interface w/glfw on the OS the binary is running on
            uint32_t glfwExtensionCount = 0;
            auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount); // The unpacked list of all glfw extensions

            if (enableValidationLayers) { // If validation layers are enabled, include the debug messenger extension as required
                extensions.push_back(vk::EXTDebugUtilsExtensionName);
            }

            return extensions;
        }

        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
            vk::DebugUtilsMessageSeverityFlagBitsEXT severity, // Severity level of the message
            vk::DebugUtilsMessageTypeFlagsEXT type, // Type of debug message
            const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, // A pointer to the debug message
            void *pUserData // nullptr; not applicable for this implementation. would contain a pointer to custom data I provide
        ) {
            std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl; // Send the debug message over to the debug messenger

            return vk::False;
        }

        void setupDebugMessenger() {
            if (!enableValidationLayers) return; // Checks if we're in debug mode or not

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

        void pickPhysicalDevice() { // What is the graphics card should Vulkan use?

            // Get all graphics cards on the system
            auto physicalDevices = instance.enumeratePhysicalDevices();
            if (physicalDevices.empty()) { // If nothing supports Vulkan, end the execution
                throw std::runtime_error("failed to find GPUs with Vulkan support!");
            }

            // Check each graphics card and see if it is suitable. It may support Vulkan generally, but can it do everything we want it to?
            auto const devIter = std::ranges::find_if( // Iterate over every element in physicalDevices, and run isDeviceSuitable against it with a lambda function
                physicalDevices,
                [&](auto const &physicalDevice) {
                    return isDeviceSuitable(physicalDevice);
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
        
        // Test each device to confirm suitability
        bool isDeviceSuitable(vk::raii::PhysicalDevice const &physicalDevice) {

            auto physicalDeviceProperties = physicalDevice.getProperties(); // Avoid duplicate function calls; saves a CPU cycle

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
            // how many machines this code can run on. HOWEVER; since this is a tutorial, I'm doing this to force a run against my GPU
            bool meetsGPURequirement = true;
            if (PREFER_DEDICATED_GPU) { // Constant defined at the top for easy shutoff
                meetsGPURequirement = (physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu);
            }

            // STEP #6: Return the device's compatibility
            return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures && meetsGPURequirement;
        }


        void mainLoop() {
            while (!glfwWindowShouldClose(window)) { // Keeps window from auto-closing on startup
                glfwPollEvents();
            }
        }


        void cleanup() {
            glfwDestroyWindow(window); // C++; need to always free what you allocate
            glfwTerminate(); // Uninitializze the library from memory
        }

    
    private:
        GLFWwindow *window = nullptr; // Pointer to the window object's place in memory
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;
        vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr; // Add a class member for the debug messenger handle
        vk::raii::PhysicalDevice  physicalDevice = nullptr;

};


// Hook for C++ to start the executable
int main() {
    // Output if a debug or release build
    #ifdef NDEBUG
        std::cout << "Release build!\n";
    #else
        std::cout << "Debug build!\n";
    #endif

    // Start up the application
    try {
        HelloTriangleApplication app;
        app.run();
    } catch (const std::exception& e) { // Print exception upon failure
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}