#include <vulkan-tutorial/sync/SyncObjects.hpp>



/***** CONSTRUCTOR AND DESTRUCTOR *****/
SyncObjects::SyncObjects(
    const vk::raii::Device &logicalDevice,
    const vk::raii::CommandBuffer& commandBuffer,
    const vk::raii::SwapchainKHR& swapchain
) {
    createSyncObjects(logicalDevice);
}

SyncObjects::~SyncObjects() {}



/***** PUBLIC METHODS *****/
const vk::raii::Fence& SyncObjects::getDrawFence() const {
    return drawFence;
}

const vk::raii::Semaphore& SyncObjects::getPresentCompleteSemaphore() const {
    return presentCompleteSemaphore;
}

const vk::raii::Semaphore& SyncObjects::getRenderFinishedSemaphore() const {
    return renderFinishedSemaphore;
}



/***** PRIVATE METHODS *****/
// Create the sync objects
void SyncObjects::createSyncObjects(const vk::raii::Device &logicalDevice) {
    presentCompleteSemaphore = vk::raii::Semaphore(logicalDevice, vk::SemaphoreCreateInfo());
    renderFinishedSemaphore = vk::raii::Semaphore(logicalDevice, vk::SemaphoreCreateInfo());
    drawFence = vk::raii::Fence(
        logicalDevice,
        {
            .flags = vk::FenceCreateFlagBits::eSignaled
        }
    );
};