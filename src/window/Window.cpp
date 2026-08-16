#include <vulkan-tutorial/window/Window.hpp>

// Standard C++ Libraries
#include <stdexcept>
#include <limits>

// External Libraries
#include <GLFW/glfw3.h>



/***** CONSTRUCTOR AND DESTRUCTOR *****/
Window::Window(uint32_t width, uint32_t height, const char* title)
{
    if (!glfwInit()) { // Ensure GLFW comes online
        throw std::runtime_error("failed to initialize GLFW");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Tell glfw to not create an OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // Enable resizing of window

    // Initialize the window; fourth param specifies monitor, next param is OpenGL specific so we don't care
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) { // Ensure window object is properly created
        glfwTerminate();
        throw std::runtime_error("failed to create GLFW window");
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

Window::~Window() {
    glfwDestroyWindow(window); // C++; need to always free what you allocate
    glfwTerminate(); // Uninitialize the library from memory
}



/***** PUBLIC METHODS *****/
// Get a pointer to the current window object
GLFWwindow* Window::getWindow() const {
    return window; 
}

// Get the required extensions for glfw
std::vector<const char*> Window::getRequiredWindowExtensions() const {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    return requiredExtensions;
};

// Get a bool for if window should close or not
bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

// Poll the window events (useful to see if user clicked a button)
void Window::pollEvents() const {
    glfwPollEvents();
}

// Create the surface that Vulkan will use
VkSurfaceKHR Window::createSurface(VkInstance instance) const {
    VkSurfaceKHR surface = nullptr;
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != 0) {
        throw std::runtime_error("failed to create window surface!");
    }
    return surface;
}


// Choose the swap extent (resolution) that the Swapchain will use
vk::Extent2D Window::chooseSwapExtent(vk::SurfaceCapabilitiesKHR const &capabilities) const {
    // Swap extent determines the width and height (resolution) of each swap chain image
    // Some platforms define the swap extent automatically; if currentExtent != UINT32_MAX, Vulkan has already chosen the size for you
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }

    // If Vulkan hasn't decided the resolution of the frames, it's up to you to decide
    int width, height;

    // Get the actual pixel dimensions of the framebuffer; may not be the same as window size
    // Window size == size in screen coordinates (logical units)
    // Framebuffer size == size in actual pixels that the GPU renders to
    glfwGetFramebufferSize(window, &width, &height);

    // Force the width and height the framebuffer is asking for into Vulkan's allowed range
    return {
        std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width), // Clamp width to within the surface's allowed range
        std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height) // Clamp height to within the surface's allowed range
    };
};


// Wait until the framebuffer is restored
void Window::waitUntilFramebufferRestored() const {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }
};

bool Window::wasFramebufferResized() const {
    return framebufferResized;
}

void Window::resetFramebufferResized() {
    framebufferResized = false;
}



/***** PRIVATE METHODS *****/
void Window::framebufferResizeCallback(
    GLFWwindow* window,
    int width,
    int height
) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (self) {
        self->framebufferResized = true;    // Set the framebuffer to true to alert
    }
};