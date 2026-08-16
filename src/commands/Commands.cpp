#include <vulkan-tutorial/commands/Commands.hpp>

// Standard C++ Libraries
#include <utility>

// Global Configs
#include <vulkan-tutorial/core/Config.hpp>



/***** CONSTRUCTOR AND DESTRUCTOR *****/
Commands::Commands(
    const Device &device
) : device(device)
{
    createCommandPool();
    createCommandBuffers();
};



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



/**** Private METHODS *****/
// Define command pool
// CPs manage the memory that is used to store the buffers
// CBs are allocated from CPs
void Commands::createCommandPool() {
    // Define the configuration info for the command pool
    vk::CommandPoolCreateInfo poolInfo{
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,        // Allow command buffers to be rerecorded individually; without this flag they all have to be reset together
        .queueFamilyIndex = device.getQueueIndex()                                      // The device queue the commands will be submitted to
    };

    // Create the command pool and tie it to the logical Device
    commandPool = vk::raii::CommandPool(device.getLogicalDevice(), poolInfo);
}

// Define command buffer
// CBs are useful as we can submit all commands to Vulkan at once; less data transfers occurring == more efficiency
// Also enables multi-threading for command recording
void Commands::createCommandBuffers() {
    // Define the allocation info for the command bufferss
    // Memory will be automatically freed when pool is destroyed, so we don't need explicit cleanup
    vk::CommandBufferAllocateInfo allocInfo{
        .commandPool = commandPool,                                         // The command pool from which to derive the memory for the buffer
        .level = vk::CommandBufferLevel::ePrimary,                          // Are allocated CBs primary or secondary? Primary can be submitted to queue, but cannot be called by other buffers. Secondary is opposite.
        .commandBufferCount = Config::MAX_FRAMES_IN_FLIGHT                  // Number of command buffers being allocated
    };

    // Allocate the new command buffers
    commandBuffers = vk::raii::CommandBuffers(device.getLogicalDevice(), allocInfo);
}

// Transition the image from one layout to another
void Commands::transitionImageLayout(
    vk::Image image,                                                    // The image we're going to modify
    vk::ImageLayout oldLayout,                                          // The current layout state
    vk::ImageLayout newLayout,                                          // The desired layout state
    vk::AccessFlags2 srcAccessMask,                                     // The memory accesses that occurred before the transition so GPU can flush caches
    vk::AccessFlags2 dstAccessMask,                                     // The memory accesses that occurred after the transition
    vk::PipelineStageFlags2 srcStageMask,                               // The pipeline stages that must finish executing before the transition can happen
    vk::PipelineStageFlags2 dstStageMask,                               // The pipeline stages that must wait to execute until the transition occurs
    const vk::raii::CommandBuffer &commandBuffer                        // The command buffer we're transitioning the images in
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