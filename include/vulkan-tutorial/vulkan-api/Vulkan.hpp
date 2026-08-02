#pragma once

// Definitions
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

// Standard C++ Libraries
#include <vector>

// External Libraries
#include <vulkan/vulkan_raii.hpp>

// Class Definition
class Vulkan {
    // Public functions
    public:
        // Constructor declaration
        Vulkan(const char *appName, const char *engine);

        // Destructor declaration
        ~Vulkan();

    // Private functions
    private:
        // Initialize Vulkan instance in memory. Allows devs to interact with the Vulkan API
        void createInstance();

        // Set up the debug messages (if running in debug mode)
        void setupDebugMessenger();


        // ***HELPER FUNCTIONS***
        // Declared as private members as they need access to Vulkan context
        std::vector<const char*> getRequiredValidationLayers();
        std::vector<const char*> getRequiredExtensions();

    
    // Private variables
    private:
        const char *appName; // Has to be const char* because of Vulkan API requirements
        const char *engine;
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr; // Vulkan instance used to call API
        vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr; // Class member for the debug messenger handle
};