#pragma once

// External Libraries
#include <vulkan/vulkan_raii.hpp>

class SyncObjects {
    // Public methods
    public:
        // Constructor declaration
        SyncObjects(
            const vk::raii::Device& logicalDevice
        );

        // Get a reference to the drawFence object
        const vk::raii::Fence& getDrawFence() const;

        // Get a reference to the presentCompleteSemaphore object
        const vk::raii::Semaphore& getPresentCompleteSemaphore() const;

        // Get a reference to the renderFinishedSemaphore object
        const vk::raii::Semaphore& getRenderFinishedSemaphore() const;


    // Private methods
    private:
        // Create the sync objects
        void createSyncObjects(const vk::raii::Device& logicalDevice);


    // Private variables
    private:
        vk::raii::Semaphore presentCompleteSemaphore = nullptr;
        vk::raii::Semaphore renderFinishedSemaphore = nullptr;
        vk::raii::Fence drawFence = nullptr;
};