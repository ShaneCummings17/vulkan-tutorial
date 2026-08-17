#pragma once

// Standard C++ Libraries
#include <array>

// External Libraries
#include <vulkan/vulkan_raii.hpp>
#include <glm/glm.hpp> // Good linear algebra library

// Struct Definitions
struct Vertex {
    glm::vec2 pos;                  // Position of the vertex
    glm::vec3 color;                // Color data of the vertex

    static vk::VertexInputBindingDescription getBindingDescription() {              // A vertex binding describes the rate at which to load data from memory throughout the vertices
        return {
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = vk::VertexInputRate::eVertex               // Move to the next data entry after each vertex
        };
    }

    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {              // Describes how to extract a vertex attribute from a chunk of vertex data originating from a binding description
        return {
            {
                {                                                                                       // Position
                    .location = 0,
                    .binding = 0,
                    .format = vk::Format::eR32G32Sfloat,
                    .offset = offsetof(Vertex, pos)
                },
                {                                                                                       // Color
                    .location = 1,
                    .binding = 0,
                    .format = vk::Format::eR32G32B32Sfloat,
                    .offset = offsetof(Vertex, color)
                }
            }
        };
    }
};