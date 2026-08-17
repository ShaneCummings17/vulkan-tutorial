#pragma once

// Standard C++ Libraries
#include <cstdint>

// Internal Libraries
#include <vulkan-tutorial/window/Window.hpp>
#include <vulkan-tutorial/vulkan-context/VulkanContext.hpp>
#include <vulkan-tutorial/devices/Device.hpp>
#include <vulkan-tutorial/swapchain/Swapchain.hpp>
#include <vulkan-tutorial/graphics-pipeline/GraphicsPipeline.hpp>
#include <vulkan-tutorial/commands/Commands.hpp>
#include <vulkan-tutorial/sync/SyncObjects.hpp>
#include <vulkan-tutorial/buffers/Buffer.hpp>

class Renderer {
    // Public methods
    public:
        // Constructor declaration
        explicit Renderer(
            const char *appName,
            const char *engine,
            Window &window
        );

        void drawFrame();
        void waitIdle();
    
    
    // Private methods
    private:
        void recordCommandBuffer(
            const vk::raii::CommandBuffer& commandBuffer,
            uint32_t imageIndex
        );
    
    
    // Private variables
    private:
        VulkanContext vulkan;
        Device device;
        Swapchain swapchain;
        GraphicsPipeline graphicsPipeline;
        Commands commands;
        SyncObjects syncObjects;
        Window& window;
        Buffer vertexBuffer;

        uint32_t frameIndex = 0; // The frame we're on

        
};