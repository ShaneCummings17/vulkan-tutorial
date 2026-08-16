#pragma once

// Internal libraries
#include <vulkan-tutorial/window/Window.hpp>
#include <vulkan-tutorial/renderer/Renderer.hpp>

class Application {
    // Public methods
    public:
        // Constructor declaration
        Application();

        // Run the application
        void run();


    // Private variables
    private:
        Window window;
        Renderer renderer;
};