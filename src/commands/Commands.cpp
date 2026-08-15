#include <vulkan-tutorial/commands/Commands.hpp>

// Standard C++ Libraries
#include <utility>


/***** CONSTRUCTOR AND DESTRUCTOR *****/
Commands::Commands(
    const vk::raii::Device &logicalDevice,
    uint32_t queueIndex
) : logicalDevice(logicalDevice)
{
    createCommandPool(queueIndex);
    createCommandBuffer();
};

Commands::~Commands() {}



/**** PUBLIC METHODS *****/
const vk::raii::CommandPool& Commands::getCommandPool() const {
    return commandPool;
}

const std::vector<vk::raii::CommandBuffer>& Commands::getCommandBuffers() const {
    return commandBuffers;
};

const vk::raii::CommandBuffer& Commands::getCommandBuffer(size_t index) const {
    return commandBuffers.at(index);
}

void Commands::recordCommandBuffer(size_t commandBufferIndex, uint32_t imageIndex, const Swapchain& swapchain, const vk::raii::Pipeline& graphicsPipeline) const {
    // Variables for later
    vk::Extent2D swapchainExtent = swapchain.getSwapchainExtent();


    // STEP #1: Begin recording to command buffer
    // Get the specified command buffer by index
    const vk::raii::CommandBuffer &commandBuffer = commandBuffers.at(commandBufferIndex);
    commandBuffer.begin({});


    // STEP #2: Transition the image layout to one that is suitable for rendering
    // Transition the image layout from vk::ImageLayout::eUndefined to vk::ImageLayout::eColorAttachmentOptional
    auto image = swapchain.getSwapchainImages()[imageIndex];
    transitionImageLayout(
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
            .extent = swapchainExtent
        },
        .layerCount = 1,                                                // Number of layers to render to; 1 for non-layered image
        .colorAttachmentCount = 1,                                      // Number of color attachments
        .pColorAttachments = &attachmentInfo                            // The color attachment to render to
    };


    // STEP #5: Begin rendering
    commandBuffer.beginRendering(renderingInfo);

    
    // STEP #6: Bind the graphics pipeline
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline);


    // STEP #7: Set values for dynamic rendering
    commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapchainExtent.width), static_cast<float>(swapchainExtent.height), 0.0f, 1.0f));
    commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapchainExtent));


    // STEP #8: Issue the draw commmand for the triangle!
    commandBuffer.draw(3, 1, 0, 0);


    // STEP #9: End rendering
    commandBuffer.endRendering();


    // STEP #10: Transition the image layourt BACK to vk::ImageLayout::ePresentSrcKHR so it can be presented to the screen
    transitionImageLayout(
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



/**** Private METHODS *****/
// Define command pool
// CPs manage the memory that is used to store the buffers
// CBs are allocated from CPs
void Commands::createCommandPool(uint32_t queueIndex) {
    // Define the configuration info for the command pool
    vk::CommandPoolCreateInfo poolInfo{
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,        // Allow command buffers to be rerecorded individually; without this flag they all have to be reset together
        .queueFamilyIndex = queueIndex                                      // The device queue the commands will be submitted to
    };

    // Create the command pool and tie it to the logical Device
    commandPool = vk::raii::CommandPool(logicalDevice, poolInfo);
}

// Define command buffer
// CBs are useful as we can submit all commands to Vulkan at once; less data transfers occurring == more efficiency
// Also enables multi-threading for command recording
void Commands::createCommandBuffer() {
    // Define the allocation info for the command buffer
    // Memory will be automatically freed when pool is destroyed, so we don't need explicit cleanup
    vk::CommandBufferAllocateInfo allocInfo{
        .commandPool = commandPool,                                         // The command pool from which to derive the memory for the buffer
        .level = vk::CommandBufferLevel::ePrimary,                          // Are allocated CBs primary or secondary? Primary can be submitted to queue, but cannot be called by other buffers. Secondary is opposite.
        .commandBufferCount = 1                                             // Number of command buffers being allocated
    };

    // Allocate a new command buffer
    auto commandBuffer = vk::raii::CommandBuffers(logicalDevice, allocInfo);

    // Insert the command buffer into the command_buffers object
    commandBuffers.insert(
        commandBuffers.end(),
        std::make_move_iterator(commandBuffer.begin()),
        std::make_move_iterator(commandBuffer.end())
    );
}

void Commands::transitionImageLayout(
    vk::Image image,                                                    // The index of the image layout to modify
    vk::ImageLayout oldLayout,                                          // The current layout state
    vk::ImageLayout newLayout,                                          // The desired layout state
    vk::AccessFlags2 srcAccessMask,                                     // The memory accesses that occurred before the transition so GPU can flush caches
    vk::AccessFlags2 dstAccessMask,                                     // The memory accesses that occurred after the transition
    vk::PipelineStageFlags2 srcStageMask,                               // The pipeline stages that must finish executing before the transition can happen
    vk::PipelineStageFlags2 dstStageMask,                               // The pipeline stages that must wait to execute until the transition occurs
    const vk::raii::CommandBuffer &commandBuffer                       // The command buffer we're transitioning the images in
) const {
    // Initiate a Vulkan 2.0 structure to tell the GPU that a specific image's layout and access permissions are changing
    vk::ImageMemoryBarrier2 barrier = {
        .srcStageMask = srcStageMask,                                   // The pipeline stages that must finish executing before the transition can happen
        .srcAccessMask = srcAccessMask,                                 // The memory accesses that occurred before the transition so GPU can flush caches
        .dstStageMask = dstStageMask,                                   // The pipeline stages that must wait to execute until the transition occurs
        .dstAccessMask = dstAccessMask,                                 // The memory accesses that occurred after the transition
        .oldLayout = oldLayout,                                         // The current layout state
        .newLayout = newLayout,                                         // The desired layout state
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,                 // Tell the GPU that the ownership of the image is not being transferred between different hardware queue families
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,                 // Tell the GPU that the ownership of the image is not being transferred between different hardware queue families
        .image = image,                                                 // Get the exact vk::Image being modified
        .subresourceRange = {                                           // Defines which parts of the image are being modified
            .aspectMask = vk::ImageAspectFlagBits::eColor,              // Only modify color
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    // Bundle the barriers together; only one barrier, so not much bundling
    vk::DependencyInfo dependencyInfo = {
        .dependencyFlags = {},
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &barrier
    };

    // Submit the barrier into the active command buffer
    commandBuffer.pipelineBarrier2(dependencyInfo);
};