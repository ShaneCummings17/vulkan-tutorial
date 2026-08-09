#pragma once

// Macros
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

// Standard C++ libraries
#include <vector>

// External Libraries
#include <vulkan/vulkan_raii.hpp>

// Required device extensions
inline const std::vector<const char*> requiredDeviceExtensions = {
    vk::KHRSwapchainExtensionName
};