#pragma once

#include <array>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.h"


class Memory {
public:
    Memory(PhysicalDevice &physicalDevice, VkDevice &dev, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    ~Memory();
    void copyData(const void *newData);


    /*
     * submit drawing commands to queue
     */
    void render(VkCommandBuffer &commandBuffer, uint32_t count);

private:
    VkDevice &device;
    VkDeviceSize bufferSize;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkMemoryRequirements memRequirements;


};
