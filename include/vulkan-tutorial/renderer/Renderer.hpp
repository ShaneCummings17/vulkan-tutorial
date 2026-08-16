#pragma once

// Standard C++ libraries
#include <vector>
#include <memory>

// Internal libraries
#include <vulkan-tutorial/window/Window.hpp>
#include <vulkan-tutorial/vulkan-api/Vulkan.hpp>
#include <vulkan-tutorial/devices/Device.hpp>
#include <vulkan-tutorial/swapchain/Swapchain.hpp>
#include <vulkan-tutorial/graphics-pipeline/GraphicsPipeline.hpp>
#include <vulkan-tutorial/commands/Commands.hpp>
#include <vulkan-tutorial/sync/SyncObjects.hpp>

class Renderer {
    // Public methods
    public:
        // Constructor declaration
        explicit Renderer(
            const char *appName,
            const char *engine,
            const Window &window
        );

        void drawFrame();
        void waitIdle();
    
    
    // Private variables
    private:
        Vulkan vulkan;
        Device device;
        Swapchain swapchain;
        GraphicsPipeline graphicsPipeline;
        Commands commands;
        SyncObjects syncObjects;
};