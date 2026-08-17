#include <vulkan-tutorial/buffers/Buffer.hpp>

// Standard C++ libraries
#include <stdexcept>



/***** CONSTRUCTOR AND DESTRUCTOR *****/
Buffer::Buffer(
    Device& device,
    const std::vector<Vertex> vertices
) :
    device(device)
{
    createVertexBuffer(vertices);
};



/**** PUBLIC METHODS *****/
const vk::Buffer& Buffer::getBuffer() const {
    return *vertexBuffer;
};


/**** PRIVATE METHODS *****/
void Buffer::createVertexBuffer(
    const std::vector<Vertex> vertices
) {
    // Create info about the buffer
    vk::BufferCreateInfo bufferInfo{
        .size = sizeof(vertices[0]) * vertices.size(),
        .usage = vk::BufferUsageFlagBits::eVertexBuffer,
        .sharingMode = vk::SharingMode::eExclusive
    };

    vertexBuffer = vk::raii::Buffer(device.getLogicalDevice(), bufferInfo);

    // Get memory requirements for the buffer
    // size == The size of the required memory in bytes
    // alignment == The offset in bytes where the buffer begins in the allocated region of memory
    // memoryTypeBits == Bit field of the memory types that are suitable for the buffer
    vk::MemoryRequirements memRequirements = vertexBuffer.getMemoryRequirements();

    // Allocate the memory based on requirements
    vk::MemoryAllocateInfo memoryAllocateInfo{
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType(
            memRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent        // Ensure that the mapped memory always matches the contents of the allocated memory
        )
    };

    vertexBufferMemory = vk::raii::DeviceMemory(device.getLogicalDevice(), memoryAllocateInfo);

    // Associate the allocated memory with the buffer
    // second param == offset within region of memory
    vertexBuffer.bindMemory(*vertexBufferMemory, 0);

    // Fill the vertex buffer with the vertex data
    // Done by mapping the buffer memory into CPU accessible memory
    void* data = vertexBufferMemory.mapMemory(0, bufferInfo.size);
    memcpy(data, vertices.data(), bufferInfo.size);
    vertexBufferMemory.unmapMemory();
};

uint32_t Buffer::findMemoryType(
    uint32_t typeFilter,
    vk::MemoryPropertyFlags properties
) {
    // Find the memory types supported by the GPU
    vk::PhysicalDeviceMemoryProperties memProperties = device.getPhysicalDevice().getMemoryProperties();

    // Find a memory type suitable for the given buffer
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    };

    throw std::runtime_error("failed to find suitable memory type!");
};