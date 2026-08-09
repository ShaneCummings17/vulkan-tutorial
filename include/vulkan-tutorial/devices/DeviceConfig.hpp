#pragma once

// Standard C++ libraries
#include <vector>

// External Libraries
#include <vulkan/vulkan_raii.hpp>

// Required device extensions
inline const std::vector<const char*> requiredDeviceExtensions = {
    vk::KHRSwapchainExtensionName
};