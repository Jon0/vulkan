#pragma once

#include <array>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.h"


class Memory {
public:
    Memory(VkPhysicalDevice &physicalDevice, VkDevice &dev);
    ~Memory();
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    /*
     * submit drawing commands to queue
     */
    void render(VkCommandBuffer &commandBuffer);

private:
    VkDevice &device;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkMemoryRequirements memRequirements;
    VkPhysicalDeviceMemoryProperties memProperties;


};
