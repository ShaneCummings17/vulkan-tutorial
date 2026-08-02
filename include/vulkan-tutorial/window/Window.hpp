#pragma once

#define GLFW_INCLUDE_VULKAN

// Standard C++ Libraries
#include <cstdint>

// External Libraries
#include <GLFW/glfw3.h>

class Window {
    // Public functions
    public:
        // Constructor declaration
        Window(uint32_t width, uint32_t height, const char* title);

        // Destructor declaration
        ~Window();

        // Get a pointer to the current window object
        GLFWwindow* getWindow() const; // Const at end; this function returns window but does not modify it
    
    // Private variables
    private:
        GLFWwindow* window = nullptr;
        uint32_t width; // Window width
        uint32_t height; // Window height
        const char* title; // Window title
};