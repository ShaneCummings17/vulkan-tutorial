#include <vulkan-tutorial/sync/SyncObjects.hpp>

// Global Configs
#include <vulkan-tutorial/core/Config.hpp>



/***** CONSTRUCTOR AND DESTRUCTOR *****/
SyncObjects::SyncObjects(
    const vk::raii::Device& logicalDevice,
    const std::vector<vk::Image>& swapchainImages
) {
    createSyncObjects(logicalDevice, swapchainImages);
}



/***** PUBLIC METHODS *****/
const std::vector<vk::raii::Fence>& SyncObjects::getInFlightFences() const {
    return inFlightFences;
}

const std::vector<vk::raii::Semaphore>& SyncObjects::getPresentCompleteSemaphores() const {
    return presentCompleteSemaphores;
}

const std::vector<vk::raii::Semaphore>& SyncObjects::getRenderFinishedSemaphores() const {
    return renderFinishedSemaphores;
}



/***** PRIVATE METHODS *****/
// Create the sync objects
void SyncObjects::createSyncObjects(
    const vk::raii::Device &logicalDevice,
    const std::vector<vk::Image>& swapchainImages

) {
    // Ensure the everything is empty; don't want to create sync objects if others already exist!
    assert(presentCompleteSemaphores.empty() && renderFinishedSemaphores.empty());

    // Create one renderFinished semaphore for every image in the swapchain
    for (size_t i = 0; i < swapchainImages.size(); i++) {
        renderFinishedSemaphores.emplace_back(logicalDevice, vk::SemaphoreCreateInfo());;
    };

    // Create CPU pacing objects based on MAX_FRAMES_IN_FLIGHT
    for (size_t i = 0; i < Config::MAX_FRAMES_IN_FLIGHT; i++) {
        presentCompleteSemaphores.emplace_back(logicalDevice, vk::SemaphoreCreateInfo());
        inFlightFences.emplace_back(logicalDevice, vk::FenceCreateInfo{
            .flags = vk::FenceCreateFlagBits::eSignaled
        });
    };
};