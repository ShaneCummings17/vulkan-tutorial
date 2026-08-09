#pragma once

// Standard C++ Libraries
#include <cstdint>

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

        // Get the command buffer
        const vk::raii::CommandBuffer& getCommandBuffer() const;


    // Private methods
    private:
        void createCommandPool(uint32_t queueIndex);
        void createCommandBuffer();
        void recordCommandBuffer(uint32_t imageIndex);


    // Private variables
    private:
        vk::raii::CommandPool commandPool = nullptr;
        vk::raii::CommandBuffer commandBuffer = nullptr;
        const vk::raii::Device& logicalDevice;
};