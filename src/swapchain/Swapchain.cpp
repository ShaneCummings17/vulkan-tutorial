#pragma once

// External Libraries
#include <vulkan/vulkan_raii.hpp>

class LogicalDevice {
    // Public functions
    public:
        // Constructor declaration
        explicit LogicalDevice(const vk::raii::PhysicalDevice &physicalDevice, vk::raii::SurfaceKHR &surface);

        // Destructor declaration
        ~LogicalDevice();

        // Get the logical device object
        const vk::raii::Device& getLogicalDevice() const;

        // Get the device graphics queue
        const vk::raii::Queue& getGraphicsQueue() const;

    // Private functions
    private:
        // Create the logical device
        void createLogicalDevice(vk::raii::PhysicalDevice const &physicalDevice, const vk::raii::SurfaceKHR &surface);


    // Private variables
    private:
        vk::raii::Device logicalDevice = nullptr; // The logical device the program is running on; i.e., the application's interface to the hardware
        vk::raii::Queue graphicsQueue = nullptr; // A pointer to the graphics queue leveraged by the logical device
};