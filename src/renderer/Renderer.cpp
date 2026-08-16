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
    recordCommandBuffer(commands.getCommandBuffer(0), imageIndex);

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


    // STEP #3: Set up the color attachment
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
        .renderArea = {                                                 // Define size of the render area
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
    commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapchain.getSwapchainExtent().width), static_cast<float>(swapchain.getSwapchainExtent().height), 0.0f, 1.0f));
    commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapchain.getSwapchainExtent()));


    // STEP #8: Issue the draw commmand for the triangle!
    commandBuffer.draw(3, 1, 0, 0);


    // STEP #9: End rendering
    commandBuffer.endRendering();


    // STEP #10: Transition the image layourt BACK to vk::ImageLayout::ePresentSrcKHR so it can be presented to the screen
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