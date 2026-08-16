#include <vulkan-tutorial/app/Application.hpp>

// Standard C++ libraries
#include <cstdint>
#include <stdexcept>



/***** CONSTRUCTOR AND DESTRUCTOR *****/
Application::Application() :
    window(800, 600, "Vulkan"),
    renderer("Hello Triangle", "No Engine", window)
{}



/***** PUBLIC METHODS *****/
// Run the application
void Application::run() {
    while (!window.shouldClose()) { // Keeps window from auto-closing on startup
        window.pollEvents();
        renderer.drawFrame();
    }

    renderer.waitIdle();
}



/***** PRIVATE METHODS *****/