#include <vulkan-tutorial/window/Window.hpp>

// External Libraries
#include <GLFW/glfw3.h>

// Constructor
Window::Window(uint32_t width, uint32_t height, const char* title) :
    width(width),
    height(height),
    title(title)
{
    glfwInit(); // Create a glfw instance so we can interact with the API
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Tell glfw to not create an OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Disable resizing of window

    // Initialize the window; fourth param specifies monitor, next param is OpenGL specific so we don't care
    window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
}

// Destructor
Window::~Window() {
    glfwDestroyWindow(window); // C++; need to always free what you allocate
    glfwTerminate(); // Uninitialize the library from memory
}

// Get a pointer to the current window object
GLFWwindow* Window::getWindow() const {
    return window; 
}

// Get a bool for if window should close or not
bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

// Poll the window events (useful to see if user clicked a button)
void Window::pollEvents() const {
    glfwPollEvents();
}