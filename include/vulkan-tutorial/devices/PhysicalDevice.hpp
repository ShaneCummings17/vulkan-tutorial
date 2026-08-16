#pragma once

// External Libraries
#include <vulkan/vulkan_raii.hpp>

class PhysicalDevice {
    // Public methods
    public:
        // Constructor declaration
        explicit PhysicalDevice(const vk::raii::Instance &instance);

        // Get the current PhysicalDevice object
        const vk::raii::PhysicalDevice& getPhysicalDevice() const;

    // Private methods
    private:
        // Pick the physical device
        void pickPhysicalDevice(const vk::raii::Instance &instance);


    // Private variables
    private:
        vk::raii::PhysicalDevice physicalDevice = nullptr; // The hardware the program is running against
};