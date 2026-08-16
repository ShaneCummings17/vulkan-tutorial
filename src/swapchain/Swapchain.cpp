#include <vulkan-tutorial/swapchain/Swapchain.hpp>

// Standard C++ Libraries
#include <cstdint>
#include <vector>
#include <algorithm>

// Local helper functions
namespace {
    uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const &capabilities) {
        // Default to using triple buffering (or larger if the GPU requires it)
        auto minImageCount = std::max(3u, capabilities.minImageCount);

        // If the GPU requires smaller than 3, use that instead
        if ((0 < capabilities.maxImageCount) && (capabilities.maxImageCount < minImageCount)) {
            minImageCount = capabilities.maxImageCount;
        }

        // Return the number of images our swap chain will be using
        return minImageCount;
    }

    vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const &availablePresentModes) {
        // Check that at LEAST eFifo (VSync) exists
        assert(
            std::ranges::any_of(
                availablePresentModes,
                [](auto presentMode) {
                    return presentMode == vk::PresentModeKHR::eFifo;
                }
            )
        );

        // Check if eMailbox (triple buffering) exists; if not, default to VSync
        return std::ranges::any_of(
            availablePresentModes,
            [](const vk::PresentModeKHR presentMode) {
                return vk::PresentModeKHR::eMailbox == presentMode;
            }
        ) ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
    }

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const &availableFormats) {
        // Find if an available surface format that supports SRGB
        const auto formatIt = std::ranges::find_if(
            availableFormats,
            [](const auto &format) {
                return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
            }
        );

        // If an SRGB format does not exist, just return the first one from the available list
        return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
    }
}



/***** CONSTRUCTOR AND DESTRUCTOR *****/
Swapchain::Swapchain(
    const Device& device,
    const vk::raii::SurfaceKHR& surface,
    const Window& window
) : device(device)
{
    createSwapchain(surface, window);
    createImageViews();
}



/***** PUBLIC METHODS *****/
const vk::raii::SwapchainKHR& Swapchain::getSwapchain() const {
    return swapchain;
}

const vk::Extent2D& Swapchain::getSwapchainExtent() const {
    return swapchainExtent;
}

const vk::SurfaceFormatKHR& Swapchain::getSwapchainSurfaceFormat() const {
    return swapchainSurfaceFormat;
}

const std::vector<vk::Image>& Swapchain::getSwapchainImages() const {
    return swapchainImages;
}

const std::vector<vk::raii::ImageView>& Swapchain::getSwapchainImageViews() const {
    return swapchainImageViews;
}



/***** PRIVATE METHODS *****/
void Swapchain::createSwapchain(
    const vk::raii::SurfaceKHR &surface,
    const Window &window
)
{
    // Get the physical device
    vk::raii::PhysicalDevice physicalDevice = device.getPhysicalDevice();

    // Get the surface capabilities (i.e., what does the window system allow you to do with a swapchain)
    vk::SurfaceCapabilitiesKHR capabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);

    // Get the swap chain extent (resolution of the frames)
    swapchainExtent = window.chooseSwapExtent(capabilities);

    // Get the image count the swap chain will use (i.e., 2 images in chain at once, 3 images in chain at once, 4 images in chain at once, etc.)
    uint32_t minImageCount = chooseSwapMinImageCount(capabilities) + 1;

    // Choose the present mode (i.e., double buffering, triple buffering, etc.)
    std::vector<vk::PresentModeKHR> availablePresentModes = physicalDevice.getSurfacePresentModesKHR(*surface);
    auto swapChainPresentMode = chooseSwapPresentMode(availablePresentModes);

    // Choose the swap chain surface format (how the pixel data is laid out) and how colors are interpreted (i.e., SRGB, HDR color spaces, etc.)
    std::vector<vk::SurfaceFormatKHR> availableFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
    swapchainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);

    
    // Create the swap chain
    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
        .surface = *surface,                                        // The surface we're rendering to
        .minImageCount = minImageCount,                             // The number of images we want the swap chain to use
        .imageFormat = swapchainSurfaceFormat.format,               // The format is which the pixel data is laid out
        .imageColorSpace = swapchainSurfaceFormat.colorSpace,       // The color space we're using
        .imageExtent = swapchainExtent,                             // The frame resolution
        .imageArrayLayers = 1,                                      // The number of layers each image consists of (always 1 unless doing stereoscopic images)
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,     // What kind of operations we're going to use the images in the swap chain for. We're rendering right to them; can also render to a separate image first for post-processing effects
        .imageSharingMode = vk::SharingMode::eExclusive,            // How to handle swap chain images that might be used across multiple queue families; eExclusive == owned by one queue family at a time (best performance)
        .preTransform = capabilities.currentTransform,              // Transformations allowed to images (ex. rotation by 90 degrees)
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,   // Specifies if the alpha channel should be used for blending with other windows; normally will want to ignore this
        .presentMode = swapChainPresentMode,                        // The buffer mode; generally choosing between triple and double buffering <VSync>
        .clipped = true,                                            // Whether or not we care about the color of obscured pixels (true == we do not)
        .oldSwapchain = nullptr                                     // The previous swap chain this one is created from; useful for window resizing when the swap chain needs to be recreated from scratch
    };

    swapchain = vk::raii::SwapchainKHR(device.getLogicalDevice(), swapChainCreateInfo);
    swapchainImages = swapchain.getImages();
}

void Swapchain::createImageViews() {
    // The viewType, format, and subresourceRange are identical for each image view

    // Make sure our image views vector is empty before we begin
    assert(swapchainImageViews.empty());

    // Create the info block as per usual
    vk::ImageViewCreateInfo imageViewCreateInfo{
        .viewType = vk::ImageViewType::e2D,                                     // viewType; specify we're rendering to a 2d screen (most cases)
        .format = swapchainSurfaceFormat.format,                                // format; how the colorspace components are configured so we getr the right color format in renders
        .subresourceRange = {                                                   // subresourceRange; describes what the image's purpose is and which part of the image should be accessed. Our images will be used as color targets w/o mipmapping for right now.
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .levelCount = 1,
            .layerCount = 1
        },      
    };

    // Iterate over all swap chain images and add them to the structure
    // I.e., grab the swap chain's 3 "image" blocks (assuming triple buffering)
    // We'll render directly to these blocks as our output
    for (auto &image : swapchainImages) {
        imageViewCreateInfo.image = image;
        swapchainImageViews.emplace_back(device.getLogicalDevice(), imageViewCreateInfo); // Add object to the end of the vector
    };
}