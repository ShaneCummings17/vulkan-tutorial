#pragma once

// Standard C++ Libraries
#include <cstdint>
#include <vector>

// Internal Libraries
#include <vulkan-tutorial/devices/Device.hpp>
#include <vulkan-tutorial/buffers/VertexDefinition.hpp>

// External Libraries
#include <vulkan/vulkan_raii.hpp>

class Buffer {
    // Public methods
    public:
        // Constructor declaration
        explicit Buffer(
            Device& device,
            const std::vector<Vertex> vertices
        );

        // Get buffer
        const vk::Buffer& getBuffer() const;
    

    // Private methods
    private:
        // Create vertex buffer
        void createVertexBuffer(
            const std::vector<Vertex> vertices
        );

        // Find the memory types supported by the graphics card
        uint32_t findMemoryType(
            uint32_t typeFilter,
            vk::MemoryPropertyFlags properties
        );
    
    // Private variables
    private:
        vk::raii::Buffer vertexBuffer = nullptr;                // The vertex buffer being processed
        vk::raii::DeviceMemory vertexBufferMemory = nullptr;    // The memory allocated for the vertex buffer
        Device& device;
};