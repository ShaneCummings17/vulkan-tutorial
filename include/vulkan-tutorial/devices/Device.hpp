#pragma once

// External Libraries
#include <vulkan/vulkan_raii.hpp>

class Device {
    // Public methods
    public:
        // Constructor declaration
        explicit Device(
            const vk::raii::Instance& instance,
            const vk::raii::SurfaceKHR& surface
        );

        // Get the physical device object
        const vk::raii::PhysicalDevice& getPhysicalDevice() const;

        // Get the logical device object
        const vk::raii::Device& getLogicalDevice() const;

        // Get the graphics queue
        const vk::raii::Queue& getGraphicsQueue() const;

        // Get the queueIndex
        uint32_t getQueueIndex() const;

    // Private methods
    private:
        // Pick the physical device
        void pickPhysicalDevice(
            const vk::raii::Instance& instance
        );
    
        // Create the logical device
        void createLogicalDevice(
            const vk::raii::SurfaceKHR &surface
        );


    // Private variables
    private:
        vk::raii::PhysicalDevice physicalDevice = nullptr; // The hardware the program is running against
        vk::raii::Device logicalDevice = nullptr; // The logical device the program is running on; i.e., the application's interface to the hardware
        vk::raii::Queue graphicsQueue = nullptr; // A pointer to the graphics queue leveraged by the logical device
        uint32_t queueIndex = UINT32_MAX;
};