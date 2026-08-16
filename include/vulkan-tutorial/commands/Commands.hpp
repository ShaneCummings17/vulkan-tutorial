#pragma once

// Standard C++ Libraries
#include <cstdint>
#include <vector>

// Internal Libraries
#include <vulkan-tutorial/swapchain/Swapchain.hpp>
#include <vulkan-tutorial/devices/Device.hpp>

// External Libraries
#include <vulkan/vulkan_raii.hpp>

class Commands {
    // Public methods
    public:
        // Constructor declaration
        explicit Commands(
            const Device& device
        );

        // Get command pool
        const vk::raii::CommandPool& getCommandPool() const;

        // Get all command buffers
        const std::vector<vk::raii::CommandBuffer>& getCommandBuffers() const;

        // Get a command buffer by index
        const vk::raii::CommandBuffer& getCommandBuffer(size_t index) const;
       
        // Transition image layout
        void transitionImageLayout(
            vk::Image image,
            vk::ImageLayout oldLayout,
            vk::ImageLayout newLayout,
            vk::AccessFlags2 srcAccessMask,
            vk::AccessFlags2 dstAccessMask,
            vk::PipelineStageFlags2 srcStageMask,
            vk::PipelineStageFlags2 dstStageMask,
            const vk::raii::CommandBuffer &commandBuffer
        ) const;


    // Private methods
    private:
        // Create command pool
        void createCommandPool();

        // Create command buffer
        void createCommandBuffer();


    // Private variables
    private:
        vk::raii::CommandPool commandPool = nullptr;
        std::vector<vk::raii::CommandBuffer> commandBuffers;
        const Device& device;
};