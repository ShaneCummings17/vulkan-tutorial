#include <vulkan-tutorial/commands/Commands.hpp>

// Standard C++ Libraries
#include <utility>



/***** CONSTRUCTOR AND DESTRUCTOR *****/
Commands::Commands(
    const vk::raii::Device &logicalDevice,
    uint32_t queueIndex
) : logicalDevice(logicalDevice) {
    createCommandPool(queueIndex);
};

Commands::~Commands() {}



/**** PUBLIC METHODS *****/
const vk::raii::CommandPool& Commands::getCommandPool() const {
    return commandPool;
}

const vk::raii::CommandBuffer& Commands::getCommandBuffer() const {
    return commandBuffer;
}



/**** Private METHODS *****/
void Commands::createCommandPool(uint32_t queueIndex) {
    vk::CommandPoolCreateInfo poolInfo{
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,        // Allow command buffers to be rerecorded individually; without this flag they all have to be reset together
        .queueFamilyIndex = queueIndex                                      // The device queue the commands will be submitted to
    };

    commandPool = vk::raii::CommandPool(logicalDevice, poolInfo);
}

void Commands::createCommandBuffer() {
    vk::CommandBufferAllocateInfo allocInfo{
        .commandPool = commandPool,
        .level = vk::CommandBufferLevel::ePrimary,          // Are allocated CBs priamry or secondary?
        .commandBufferCount = 1
    };

    commandBuffer = std::move(vk::raii::CommandBuffers(logicalDevice, allocInfo).front());
}

void Commands::recordCommandBuffer(uint32_t imageIndex) {
    // STEP #1: Begin recording to command buffer
    commandBuffer.begin({});

    
};