#include <cstring>

#include "memory.h"


Memory::Memory(PhysicalDevice &physicalDevice, VkDevice &dev, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    :
    device {dev},
    bufferSize {size} {

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);


    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = physicalDevice.findMemoryType(memRequirements.memoryTypeBits, properties);

    // ...

    if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);
}


Memory::~Memory() {
    vkFreeMemory(device, vertexBufferMemory, nullptr);
    vkDestroyBuffer(device, vertexBuffer, nullptr);
}


VkBuffer &Memory::getBuffer() {
    return vertexBuffer;
}


void Memory::copyData(const void *newData) {
    copyDataTo(newData, 0, bufferSize);
}


void Memory::copyDataTo(const void *newData, size_t offset, size_t count) {
    void* data;
    vkMapMemory(device, vertexBufferMemory, offset, count, 0, &data);
    memcpy(data, newData, (size_t) count);
    vkUnmapMemory(device, vertexBufferMemory);
}


void Memory::copyFromBuffer(DeviceQueue &queue, const Memory &mem) {
    VkCommandBuffer commandBuffer;
    queue.allocateCommandBuffers(&commandBuffer, 1);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = bufferSize;
    vkCmdCopyBuffer(commandBuffer, mem.vertexBuffer, vertexBuffer, 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);


    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    queue.submit(submitInfo, VK_NULL_HANDLE);
    queue.waitIdle();
    queue.freeCommandBuffers(&commandBuffer, 1);
}
