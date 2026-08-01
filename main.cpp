#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

// Constants
constexpr uint32_t WIDTH = 800; // Window width
constexpr uint32_t HEIGHT = 600; // Window height

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
        }

        void createInstance() {
            constexpr vk::ApplicationInfo appInfo{ // Define info about the application (Name, App Version, Engine, Engine Version, Vulkan API Version)
                .pApplicationName = "Hello Triangle",
                .applicationVersion = vk::makeVersion(1, 0, 0),
                .pEngineName = "No Engine",
                .engineVersion = vk::makeVersion(1, 0, 0),
                .apiVersion = vk::ApiVersion14
            };

            // Get the required global instance extensions from GLFW; basically tell Vulkan how to interface w/glfw on the OS the binary is running on
            uint32_t glfwExtensionCount = 0;
            auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            // Check if the required GLFW extensions are supported by the Vulkan implementation
            auto extensionProperties = context.enumerateInstanceExtensionProperties(); // Get all extensions supported by Vulkan; returns a struct
            for (uint32_t i = 0; i < glfwExtensionCount; ++i) { // For every extension glfw needs...
                if (std::ranges::none_of( // Do literally none of these extensions match the ones supported by Vulkan?
                    extensionProperties,
                    [glfwExtension = glfwExtensions[i]](auto const& extensionProperty)
                    { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; } // Get the name of the extension; compare to the required extensios
                ))
                { // If none match, throw an error. The Vulkan API can't open a window on this operating system!
                    throw std::runtime_error("Required GLFW extension not supported: " + std::string(glfwExtensions[i]));
                }
            }
            
            vk::InstanceCreateInfo createInfo{ // Tells Vulkan driver which global extensions and validation layers we want to use (apply to entire program, not a specific device)
                .pApplicationInfo = &appInfo,
                .enabledExtensionCount = glfwExtensionCount,
                .ppEnabledExtensionNames = glfwExtensions
            };


            // Finally create the damn Vulkan instance
            instance = vk::raii::Instance(context, createInfo);

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
        GLFWwindow *window = nullptr; // Pointer to the window object's place in memory'
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;

};


// Hook for C++ to start the executable
int main() {
    try {
        HelloTriangleApplication app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}