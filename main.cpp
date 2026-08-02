// Internal libraries
#include <vulkan-tutorial/app/Application.hpp>

// Standard C++ Libraries
#include <stdexcept>

// Hook for C++ to start the executable
int main() {
    // Output if a debug or release build
    #ifdef NDEBUG
        std::cout << "Release build!\n";
    #else
        std::cout << "Debug build!\n";
    #endif

    // Start up the application
    try {
        Application app;
        app.run();
    } catch (const std::exception& e) { // Print exception upon failure
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}