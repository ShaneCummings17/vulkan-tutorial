#pragma once

// Internal libraries
#include <vulkan-tutorial/window/Window.hpp>
#include <vulkan-tutorial/devices/LogicalDevice.hpp>

// External Libraries
#include <vulkan/vulkan_raii.hpp>

class Swapchain {
    // Public methods
    public:
        // Constructor declaration
        explicit Swapchain(
            const vk::raii::PhysicalDevice &physicalDevice,
            const vk::raii::SurfaceKHR &surface,
            const Window &window,
            const vk::raii::Device &logicalDevice
        );

        // Destructor declaration
        ~Swapchain();

        // Get the swapchain object
        const vk::raii::SwapchainKHR& getSwapchain() const;

        // Get the swapchain extent
        const vk::Extent2D& getSwapchainExtent() const;

        // Get the swapchain surface format
        const vk::SurfaceFormatKHR& getSwapchainSurfaceFormat() const;

    // Private methods
    private:
        // Create the logical device
        void createSwapchain(
            const vk::raii::PhysicalDevice &physicalDevice,
            const vk::raii::SurfaceKHR &surface,
            const Window &window
        );

        // Create the image views
        void createImageViews();


    // Private variables
    private:
        vk::raii::SwapchainKHR swapchain = nullptr; // The swapchain used to render frames
        vk::Extent2D swapchainExtent; // The resolution of images in the swap chain
        std::vector<vk::Image> swapchainImages; // The images in the swapchain
        vk::SurfaceFormatKHR swapchainSurfaceFormat; // The format that pixels are laid out + the color space
        std::vector<vk::raii::ImageView> swapchainImageViews; // A vector storing image views (i.e., how to access the image, which part to access, if it should be treated as a 2D depth texture, etc.)
        const vk::raii::Device& logicalDevice; // The logical device the swapchain is attached to
};