#include <vulkan-tutorial/app/Application.hpp>

// Constructor
Application::Application() :
    window(800, 600, "Vulkan")
{

}

// Destructor
Application::~Application() {}


// Run the application
void Application::run() {
    mainLoop();
}

// Keep window from auto-closing
void Application::mainLoop() {
    while (!glfwWindowShouldClose(window.getWindow())) { // Keeps window from auto-closing on startup
        glfwPollEvents();
    }
}