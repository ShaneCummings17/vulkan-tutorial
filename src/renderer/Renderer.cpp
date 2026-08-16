#include <vulkan-tutorial/renderer/Renderer.hpp>

// Standard C++ libraries
#include <vector>
#include <memory>
#include <cstdint>

// External Libraries
#include <vulkan/vulkan_raii.hpp>

// Global Config
#include <vulkan-tutorial/core/Config.hpp>

// Local variables
namespace {
    uint32_t frameIndex = 0; // Keep track of what frame we're on
}



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
    syncObjects(device.getLogicalDevice(), swapchain.getSwapchainImages())
{};



/***** PUBLIC METHODS *****/
void Renderer::waitIdle() {
    device.getLogicalDevice().waitIdle();
}

// Draw each frame
void Renderer::drawFrame() {
    // Wait for fence result 
    auto fenceResult = device.getLogicalDevice().waitForFences(
        *syncObjects.getInFlightFences()[frameIndex],                       // Fence result to wait for
        vk::True,                                                           // If true, wait for all fences
        UINT64_MAX                                                          // Disable timeout
    );
    if (fenceResult != vk::Result::eSuccess) {
        throw std::runtime_error("failed to wait for fence!");
    };

    // Reset fence after result
    device.getLogicalDevice().resetFences(*syncObjects.getInFlightFences()[frameIndex]);;

    // Grab an image from the framebuffer after the previous frame has finished
    auto [result, imageIndex] = swapchain.getSwapchain().acquireNextImage(
        UINT64_MAX,                                                         // Disable timeout for image to become available
        *syncObjects.getPresentCompleteSemaphores()[frameIndex],            // Signal the presentCompleteSemaphore after we're finished using the image
        nullptr                                                             // Don't bother signaling a fence
    );

    // Record all the commands we want sent to the buffer
    recordCommandBuffer(commands.getCommandBuffer(frameIndex), imageIndex);

    // Submit the command buffer to the GPU!
    vk::PipelineStageFlags waitDestinationStageMask(
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    );

    const vk::SubmitInfo submitInfo{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*syncObjects.getPresentCompleteSemaphores()[frameIndex],
        .pWaitDstStageMask = &waitDestinationStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &*commands.getCommandBuffer(frameIndex),
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &*syncObjects.getRenderFinishedSemaphores()[imageIndex]
    };

    const auto &queue = device.getGraphicsQueue();
    queue.submit(submitInfo, *syncObjects.getInFlightFences()[frameIndex]);

    // Submit the result back to the swapchain and have it eventually show up on the screen
    const vk::PresentInfoKHR presentInfoKHR{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*syncObjects.getRenderFinishedSemaphores()[imageIndex],
        .swapchainCount = 1,
        .pSwapchains = &*swapchain.getSwapchain(),
        .pImageIndices = &imageIndex
    };

    result = queue.presentKHR(presentInfoKHR);

    // Advance to the next frame
    frameIndex = (frameIndex + 1) % Config::MAX_FRAMES_IN_FLIGHT;
    
}



/***** PRIVATE METHODS *****/
void Renderer::recordCommandBuffer(
    const vk::raii::CommandBuffer& commandBuffer,
    uint32_t imageIndex
) {

    // STEP #1: Begin recording to command buffer
    // Get the specified command buffer by index
    commandBuffer.begin({});


    // STEP #2: Transition the image layout to one that is suitable for rendering
    // Transition the image layout from vk::ImageLayout::eUndefined to vk::ImageLayout::eColorAttachmentOptional
    auto image = swapchain.getSwapchainImages()[imageIndex];
    commands.transitionImageLayout(
        image,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        commandBuffer
    );


    // STEP #3: Set up the color attachment information
    vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
    vk::RenderingAttachmentInfo attachmentInfo = {
        .imageView = swapchain.getSwapchainImageViews()[imageIndex],    // The image view to render to
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,        // The layout the image will be in during rendering
        .loadOp = vk::AttachmentLoadOp::eClear,                         // What to do with the image before rendering (clear to black)
        .storeOp = vk::AttachmentStoreOp::eStore,                       // What to do with the image after rendering (store for later use)
        .clearValue = clearColor                                        // The color black
    };


    // STEP #4: Setup the rendering info
    vk::RenderingInfo renderingInfo = {
        .renderArea = {                                                 // Define size of the render area; we're doing size of the whole screen
            .offset = {0, 0},
            .extent = swapchain.getSwapchainExtent()
        },
        .layerCount = 1,                                                // Number of layers to render to; 1 for non-layered image
        .colorAttachmentCount = 1,                                      // Number of color attachments
        .pColorAttachments = &attachmentInfo                            // The color attachment to render to
    };


    // STEP #5: Begin rendering
    commandBuffer.beginRendering(renderingInfo);

    
    // STEP #6: Bind the graphics pipeline
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.getGraphicsPipeline());


    // STEP #7: Set values for dynamic rendering
    commandBuffer.setViewport(
        0,
        vk::Viewport(0.0f, 0.0f, static_cast<float>(swapchain.getSwapchainExtent().width),
        static_cast<float>(swapchain.getSwapchainExtent().height),
        0.0f,
        1.0f
    ));
    commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapchain.getSwapchainExtent()));


    // STEP #8: Issue the draw commmand for the triangle!
    commandBuffer.draw(3, 1, 0, 0);


    // STEP #9: End rendering
    commandBuffer.endRendering();


    // STEP #10: Transition the image layout to vk::ImageLayout::ePresentSrcKHR so it can be presented to the screen
    commands.transitionImageLayout(
        image,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite,
        {},
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eBottomOfPipe,
        commandBuffer
    );


    // STEP #11: Finish recording the command buffer
    commandBuffer.end();
};