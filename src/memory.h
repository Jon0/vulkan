#pragma once

#include <array>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.h"


class Memory {
public:
    Memory(Device &dev, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    ~Memory();

    VkBuffer &getBuffer();
    void copyData(const void *newData, size_t count);
    void copyDataTo(const void *newData, size_t offset, size_t count);

    /*
     * queues copying to device memory
     */
    void copyFromBuffer(DeviceQueue &queue, const Memory &mem);

private:
    VkDevice &device;
    VkDeviceSize bufferSize;
    VkBuffer vertexBuffer;
    VkDeviceMemory bufferMemory;
    VkMemoryRequirements memRequirements;


};
