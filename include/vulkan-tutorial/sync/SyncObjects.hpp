#pragma once

// External Libraries
#include <vulkan/vulkan_raii.hpp>
#include <vector>



class SyncObjects {
    // Public methods
    public:
        // Constructor declaration
        SyncObjects(
            const vk::raii::Device& logicalDevice,
            const std::vector<vk::Image>& swapchainImages
        );

        // Get a reference to the drawFence object
        const std::vector<vk::raii::Fence>& getInFlightFences() const;

        // Get a reference to the presentCompleteSemaphore object
        const std::vector<vk::raii::Semaphore>& getPresentCompleteSemaphores() const;

        // Get a reference to the renderFinishedSemaphore object
        const std::vector<vk::raii::Semaphore>& getRenderFinishedSemaphores() const;


    // Private methods
    private:
        // Create the sync objects
        void createSyncObjects(
            const vk::raii::Device& logicalDevice,
            const std::vector<vk::Image>& swapchainImages
        );


    // Private variables
    private:
        std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
        std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
        std::vector<vk::raii::Fence> inFlightFences;
};