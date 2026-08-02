#include <vulkan-tutorial/app/Application.hpp>

// External libraries
#include <vulkan/vulkan_raii.hpp>

// Constructor
Application::Application() :
    window(800, 600, "Vulkan"),
    vulkan("Hello Triangle", "No Engine")
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
    while (!window.shouldClose()) { // Keeps window from auto-closing on startup
        window.pollEvents();
    }
}