#pragma once

// Internal libraries
#include <vulkan-tutorial/devices/Device.hpp>
#include <vulkan-tutorial/window/Window.hpp>

// External Libraries
#include <vulkan/vulkan_raii.hpp>

class Swapchain {
    // Public methods
    public:
        // Constructor declaration
        explicit Swapchain(
            const Device &device,
            const vk::raii::SurfaceKHR &surface,
            const Window &window
        );

        // Get the swapchain object
        const vk::raii::SwapchainKHR& getSwapchain() const;

        // Get the swapchain extent
        const vk::Extent2D& getSwapchainExtent() const;

        // Get the swapchain surface format
        const vk::SurfaceFormatKHR& getSwapchainSurfaceFormat() const;

        // Get the swapchain images
        const std::vector<vk::Image>& getSwapchainImages() const;

        // Get the swapchain image views
        const std::vector<vk::raii::ImageView>& getSwapchainImageViews() const;

        // Recreate the swapchain (allows resizing of window)
        void recreateSwapchain();

    // Private methods
    private:
        // Create the swapchain
        void createSwapchain();

        // Create the image views
        void createImageViews();

        // Cleanup the swapchain (allows resizing of window)
        void cleanupSwapchain();


    // Private variables
    private:
        vk::raii::SwapchainKHR swapchain = nullptr; // The swapchain used to render frames
        vk::Extent2D swapchainExtent; // The resolution of images in the swap chain
        std::vector<vk::Image> swapchainImages; // The images in the swapchain
        vk::SurfaceFormatKHR swapchainSurfaceFormat; // The format that pixels are laid out + the color space
        std::vector<vk::raii::ImageView> swapchainImageViews; // A vector storing image views (i.e., how to access the image, which part to access, if it should be treated as a 2D depth texture, etc.)
        const Device& device; // The device the swapchain is attached to
        const Window& window; // The window the swapchain is rendering to
        const vk::raii::SurfaceKHR& surface; // The surface the swapchain is rendering to
};