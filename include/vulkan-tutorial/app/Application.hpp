#pragma once

// Internal libraries
#include <vulkan-tutorial/window/Window.hpp>
#include <vulkan-tutorial/vulkan-api/Vulkan.hpp>

// Standard C++ Libraries
#include <iostream>

class Application {
    // Public functions
    public:
        // Constructor declaration
        Application();

        // Destructor declaration
        ~Application();

        // Run the application
        void run();
    
    // Private functions
    private:
        // Keep window from auto-closing
        void mainLoop();

    // Private variables
    private:
        Window window;
        Vulkan vulkan;
};