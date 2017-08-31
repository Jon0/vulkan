#pragma once

#include <string>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class Device {
public:
    Device(VkPhysicalDevice &physDevice);
    ~Device();
    VkDevice &getVulkanDevice();
    VkQueue &getGraphicsQueue();
    int getGraphicsFamily() const;

    VkShaderModule loadShaderModule(const std::string &filepath);

    /*
     * returns initialised commandPool
     */
    void createCommandPool(VkCommandPool &commandPool);
    void createSemaphore(VkSemaphore &semaphore);

private:
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    int graphicsFamily;
    int displayFamily;
};
