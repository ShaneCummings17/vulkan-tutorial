#pragma once

// Macros
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

// Standard C++ libraries
#include <vector>

// Internal libraries
#include <vulkan-tutorial/window/Window.hpp>

// External Libraries
#include <vulkan/vulkan_raii.hpp>

class Vulkan {
    // Public functions
    public:
        // Constructor declaration
        Vulkan(
            const char *appName,
            const char *engine,
            const Window *window
        );

        // Destructor declaration
        ~Vulkan();

    // Private functions
    private:
        // Create the Vulkan instance
        // Initialize Vulkan instance in memory. Allows devs to interact with the Vulkan API
        void createInstance();

        // Get required instance extensions
        std::vector<const char*> getRequiredInstanceExtensions();


        // Create debug hooks
        // Set up the debug messages (if running in debug mode)
        void setupDebugMessenger();


        // Initialize Surface
        void initSurface();


    // Private variables
    private:
        const char *appName; // Has to be const char* because of Vulkan API requirements
        const char *engine;
        const Window *window;
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr; // Vulkan instance used to call API
        vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr; // Class member for the debug messenger handle
        vk::raii::SurfaceKHR surface = nullptr; // The surface to which graphics output will be rendered; connects the Vulkan API to the window
};