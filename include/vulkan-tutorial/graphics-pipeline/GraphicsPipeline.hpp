#pragma once

// External Libraries
#include <vulkan/vulkan_raii.hpp>

class GraphicsPipeline {
    // Public methods
    public:
        // Constructor declaration
        explicit GraphicsPipeline(
            const vk::raii::Device &logicalDevice,
            const vk::Extent2D &swapChainExtent
        );

        // Destructor declaration
        ~GraphicsPipeline();


    // Private methods
    private:
        void createGraphicsPipeline(
            const vk::raii::Device &logicalDevice,
            const vk::Extent2D &swapChainExtent
        );


    // Private variables
    private:
        vk::raii::PipelineLayout pipelineLayout = nullptr;
};