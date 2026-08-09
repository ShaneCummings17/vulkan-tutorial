#pragma once

// Internal libraries
#include <vulkan-tutorial/window/Window.hpp>
#include <vulkan-tutorial/vulkan-api/Vulkan.hpp>

class Application {
    // Public functions
    public:
        // Constructor declaration
        Application();

        // Destructor declaration
        ~Application();

        // Run the application
        void run();

    // Private variables
    private:
        Window window;
        Vulkan vulkan;
};