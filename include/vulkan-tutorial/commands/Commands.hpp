#pragma once

// Standard C++ Libraries
#include <cstdint>
#include <vector>

// Internal Libraries
#include <vulkan-tutorial/swapchain/Swapchain.hpp>

// External Libraries
#include <vulkan/vulkan_raii.hpp>

class Commands {
    // Public methods
    public:
        // Constructor declaration
        explicit Commands(
            const vk::raii::Device &logicalDevice,
            uint32_t queueIndex
        );

        // Destructor declaration
        ~Commands();

        // Get command pool
        const vk::raii::CommandPool& getCommandPool() const;

        // Get all command buffers
        const std::vector<vk::raii::CommandBuffer>& getCommandBuffers() const;

        // Get a command buffer by index
        const vk::raii::CommandBuffer& getCommandBuffer(size_t index) const;
       
        // Record the command buffer
        void recordCommandBuffer(size_t commandBufferIndex, uint32_t imageIndex, const Swapchain& swapchain, const vk::raii::Pipeline& graphicsPipeline) const;


    // Private methods
    private:
        // Create command pool
        void createCommandPool(uint32_t queueIndex);

        // Create command buffer
        void createCommandBuffer();

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


    // Private variables
    private:
        vk::raii::CommandPool commandPool = nullptr;
        std::vector<vk::raii::CommandBuffer> commandBuffers;
        const vk::raii::Device& logicalDevice;
};