#pragma once

#define GLFW_INCLUDE_VULKAN

// Standard C++ Libraries
#include <cstdint>
#include <vector>

// External Libraries
struct GLFWwindow;
typedef struct VkInstance_T* VkInstance;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;

class Window {
    // Public functions
    public:
        // Constructor declaration
        Window(uint32_t width, uint32_t height, const char* title);

        // Destructor declaration
        ~Window();

        // Get a pointer to the current window object
        GLFWwindow* getWindow() const; // Const at end; this function returns window but does not modify it

        // Get the required extensions for glfw
        std::vector<const char*> getRequiredWindowExtensions() const;

        // Get a bool for if window should close or not
        bool shouldClose() const; // Const at end; this function returns window but does not modify it

        // Poll the window events (useful to see if user clicked a button)
        void pollEvents() const;

        // Create the surface that Vulkan will use
        VkSurfaceKHR createSurface(VkInstance instance) const;
    
    // Private variables
    private:
        GLFWwindow* window = nullptr;
        uint32_t width; // Window width
        uint32_t height; // Window height
        const char* title; // Window title
};