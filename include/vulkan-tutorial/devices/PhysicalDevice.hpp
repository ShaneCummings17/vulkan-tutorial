#pragma once

// External Libraries
#include <vulkan/vulkan_raii.hpp>

class PhysicalDevice {
    // Public functions
    public:
        // Constructor declaration
        explicit PhysicalDevice(const vk::raii::Instance &instance);

        // Destructor declaration
        ~PhysicalDevice();

        // Get the current PhysicalDevice object
        const vk::raii::PhysicalDevice& getPhysicalDevice() const;

    // Private functions
    private:
        // Pick the physical device
        void pickPhysicalDevice(const vk::raii::Instance &instance);


    // Private variables
    private:
        vk::raii::PhysicalDevice physicalDevice = nullptr; // The hardware the program is running against
};