#pragma once

// External Libraries
#include <vulkan/vulkan_raii.hpp>

class GraphicsPipeline {
    // Public methods
    public:
        // Constructor declaration
        explicit GraphicsPipeline(
            const vk::raii::Device &logicalDevice,
            const vk::Extent2D &swapchainExtent,
            const vk::SurfaceFormatKHR &swapchainSurfaceFormat
        );

        // Destructor declaration
        ~GraphicsPipeline();

        // Get graphics pipeline
        const vk::raii::Pipeline& getGraphicsPipeline() const;


    // Private methods
    private:
        void createGraphicsPipeline(
            const vk::raii::Device &logicalDevice,
            const vk::Extent2D &swapchainExtent,
            const vk::SurfaceFormatKHR &swapchainSurfaceFormat
        );


    // Private variables
    private:
        vk::raii::PipelineLayout pipelineLayout = nullptr;
        vk::raii::Pipeline graphicsPipeline = nullptr;
};