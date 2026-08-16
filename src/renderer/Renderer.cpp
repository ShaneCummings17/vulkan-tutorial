#include <vulkan-tutorial/renderer/Renderer.hpp>

// Standard C++ libraries
#include <vector>
#include <memory>

// External Libraries
#include <vulkan/vulkan_raii.hpp>



/***** CONSTRUCTOR AND DESTRUCTOR *****/
Renderer::Renderer(
    const char *appName,
    const char *engine,
    const Window &window
) :
    vulkan(appName, engine, window),
    device(vulkan.getInstance(), vulkan.getSurface()),
    swapchain(device, vulkan.getSurface(), window),
    graphicsPipeline(device.getLogicalDevice(), swapchain.getSwapchainExtent(), swapchain.getSwapchainSurfaceFormat()),
    commands(device),
    syncObjects(device.getLogicalDevice())
{};



/***** PUBLIC METHODS *****/
void Renderer::waitIdle() {
    device.getLogicalDevice().waitIdle();
}

// Draw each frame
void Renderer::drawFrame() {
    // Wait for fence result 
    auto fenceResult = device.getLogicalDevice().waitForFences(
        *syncObjects.getDrawFence(),                                         // Fence result to wait for
        vk::True,                                           // If true, wait for all fences
        UINT64_MAX                                          // Disable timeout
    );
    if (fenceResult != vk::Result::eSuccess) {
        throw std::runtime_error("failed to wait for fence!");
    };

    // Reset fence after result
    device.getLogicalDevice().resetFences(*syncObjects.getDrawFence());

    // Grab an image from the framebuffer after the previous frame has finished
    auto [result, imageIndex] = swapchain.getSwapchain().acquireNextImage(
        UINT64_MAX,                                         // Disable timeout for image to become available
        *syncObjects.getPresentCompleteSemaphore(),                          // Signal the presentCompleteSemaphore after we're finished using the image
        nullptr                                             // Don't bother signaling a fence
    );

    // Record the commands we want to the buffer
    commands.recordCommandBuffer(0, imageIndex, swapchain, graphicsPipeline.getGraphicsPipeline());

    // Submit the command buffer to the GPU!
    vk::PipelineStageFlags waitDestinationStageMask(
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    );

    const vk::SubmitInfo submitInfo{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*syncObjects.getPresentCompleteSemaphore(),
        .pWaitDstStageMask = &waitDestinationStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &*commands.getCommandBuffer(0),
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &*syncObjects.getRenderFinishedSemaphore()
    };

    const auto &queue = device.getGraphicsQueue();
    queue.submit(submitInfo, *syncObjects.getDrawFence());

    // Submit the result back to the swapchain and have it eventually show up on the screen
    const vk::PresentInfoKHR presentInfoKHR{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*syncObjects.getRenderFinishedSemaphore(),
        .swapchainCount = 1,
        .pSwapchains = &*swapchain.getSwapchain(),
        .pImageIndices = &imageIndex
    };

    result = queue.presentKHR(presentInfoKHR);
    
}



/***** PRIVATE METHODS *****/