#include <vulkan-tutorial/app/Application.hpp>

// Standard C++ libraries
#include <cstdint>
#include <stdexcept>



/***** CONSTRUCTOR AND DESTRUCTOR *****/
Application::Application() :
    window(800, 600, "Vulkan"),
    vulkan("Hello Triangle", "No Engine", window)
{}



/***** PUBLIC METHODS *****/
// Run the application
void Application::run() {
    while (!window.shouldClose()) { // Keeps window from auto-closing on startup
        window.pollEvents();
        drawFrame();
    }

    vulkan.getLogicalDeviceObject().getLogicalDevice().waitIdle();
}



/***** PRIVATE METHODS *****/
// Draw each frame
void Application::drawFrame() {
    // Get the logical device
    const auto &logicalDeviceObject = vulkan.getLogicalDeviceObject();
    const auto &logicalDevice = logicalDeviceObject.getLogicalDevice();

    // Get the draw fence
    const auto &syncObject = vulkan.getSyncObjects();
    const auto &drawFence = syncObject.getDrawFence();
    
    // Wait for fence result 
    auto fenceResult = logicalDevice.waitForFences(
        *drawFence,                                         // Fence result to wait for
        vk::True,                                           // If true, wait for all fences
        UINT64_MAX                                          // Disable timeout
    );
    if (fenceResult != vk::Result::eSuccess) {
        throw std::runtime_error("failed to wait for fence!");
    };

    // Reset fence after result
    logicalDevice.resetFences(*drawFence);

    // Grab an image from the framebuffer after the previous frame has finished
    const auto &swapchain = vulkan.getSwapchainObject().getSwapchain();
    const auto &presentCompleteSemaphore = syncObject.getPresentCompleteSemaphore();
    const auto &renderFinishedSemaphore = syncObject.getRenderFinishedSemaphore();
    auto [result, imageIndex] = swapchain.acquireNextImage(
        UINT64_MAX,                                         // Disable timeout for image to become available
        *presentCompleteSemaphore,                          // Signal the presentCompleteSemaphore after we're finished using the image
        nullptr                                             // Don't bother signaling a fence
    );

    // Record the commands we want to the buffer
    const auto &commandsObject = vulkan.getCommandsObject();
    const auto &swapchainObject = vulkan.getSwapchainObject();
    const auto &graphicsPipelineObject = vulkan.getGraphicsPipelineObject();
    commandsObject.recordCommandBuffer(0, imageIndex, swapchainObject, graphicsPipelineObject.getGraphicsPipeline());

    // Submit the command buffer to the GPU!
    vk::PipelineStageFlags waitDestinationStageMask(
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    );

    const vk::SubmitInfo submitInfo{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*presentCompleteSemaphore,
        .pWaitDstStageMask = &waitDestinationStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &*commandsObject.getCommandBuffer(0),
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &*renderFinishedSemaphore
    };

    const auto &queue = logicalDeviceObject.getGraphicsQueue();
    queue.submit(submitInfo, *drawFence);

    // Submit the result back to the swapchain and have it eventually show up on the screen
    const vk::PresentInfoKHR presentInfoKHR{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*renderFinishedSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &*swapchain,
        .pImageIndices = &imageIndex
    };

    result = queue.presentKHR(presentInfoKHR);

    
}