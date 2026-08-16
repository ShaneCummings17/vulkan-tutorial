#pragma once

// Standard C++ libraries
#include <vector>
#include <memory>

// Internal libraries
#include <vulkan-tutorial/window/Window.hpp>

// External Libraries
#include <vulkan/vulkan_raii.hpp>

class Vulkan {
    // Public methods
    public:
        // Constructor declaration
        explicit Vulkan(
            const char *appName,
            const char *engine,
            const Window &window
        );

        const vk::raii::Instance& getInstance() const;
        const vk::raii::SurfaceKHR& getSurface() const;


    // Private methods
    private:
        // Create the Vulkan instance
        // Initialize Vulkan instance in memory. Allows devs to interact with the Vulkan API
        void createInstance(
            const char *appName,
            const char *engine
        );

        // Get required instance extensions
        std::vector<const char*> getRequiredInstanceExtensions();

        // Create debug hooks
        // Set up the debug messages (if running in debug mode)
        void setupDebugMessenger();

        // Initialize Surface
        void initSurface();


    // Private variables
    private:
        const Window &window;
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr; // Vulkan instance used to call API
        vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr; // Class member for the debug messenger handle
        vk::raii::SurfaceKHR surface = nullptr; // The surface to which graphics output will be rendered; connects the Vulkan API to the window
};