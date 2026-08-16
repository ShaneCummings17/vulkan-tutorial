#pragma once

// Standard C++ libraries
#include <vector>
#include <memory>

// Internal libraries
#include <vulkan-tutorial/window/Window.hpp>
#include <vulkan-tutorial/devices/Device.hpp>
#include <vulkan-tutorial/swapchain/Swapchain.hpp>
#include <vulkan-tutorial/graphics-pipeline/GraphicsPipeline.hpp>
#include <vulkan-tutorial/commands/Commands.hpp>
#include <vulkan-tutorial/sync/SyncObjects.hpp>

// External Libraries
#include <vulkan/vulkan_raii.hpp>

class Vulkan {
    // Public methods
    public:
        // Constructor declaration
        explicit Vulkan(
            const char *appName,
            const char *engine,
            const Window &window
        );

        // Expose the logical device object
        Device& getDeviceObject() const;

        // Expose the sync objects
        const SyncObjects& getSyncObjects() const;

        // Expose the swapchain object
        const Swapchain& getSwapchainObject() const;

        // Expose the commands object
        const Commands& getCommandsObject() const;

        // Expose the Graphics Pipeline object
        const GraphicsPipeline& getGraphicsPipelineObject() const;


    // Private methods
    private:
        // Create the Vulkan instance
        // Initialize Vulkan instance in memory. Allows devs to interact with the Vulkan API
        void createInstance();

        // Get required instance extensions
        std::vector<const char*> getRequiredInstanceExtensions();

        // Create debug hooks
        // Set up the debug messages (if running in debug mode)
        void setupDebugMessenger();

        // Initialize Surface
        void initSurface();


    // Private variables
    private:
        const char *appName; // Has to be const char* because of Vulkan API requirements
        const char *engine;
        const Window &window;
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr; // Vulkan instance used to call API
        vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr; // Class member for the debug messenger handle
        vk::raii::SurfaceKHR surface = nullptr; // The surface to which graphics output will be rendered; connects the Vulkan API to the window
        std::unique_ptr<Device> device; // The hardware the program is running against
        std::unique_ptr<Swapchain> swapchain; // The swapchain used to render frames
        std::unique_ptr<GraphicsPipeline> graphicsPipeline; // The graphics pipeline
        std::unique_ptr<Commands> commands; // The commands object containing both the commandPool and the commandBuffer
        std::unique_ptr<SyncObjects> syncObjects; // The semaphores and fences used for synchronization
};