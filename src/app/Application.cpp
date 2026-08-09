#include <vulkan-tutorial/app/Application.hpp>



/***** CONSTRUCTOR AND DESTRUCTOR *****/
Application::Application() :
    window(800, 600, "Vulkan"),
    vulkan("Hello Triangle", "No Engine", window)
{}

Application::~Application() {}



/***** PUBLIC METHODS *****/
// Run the application
void Application::run() {
    while (!window.shouldClose()) { // Keeps window from auto-closing on startup
        window.pollEvents();
    }
}



/***** PRIVATE METHODS *****/