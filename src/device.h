#pragma once

#include <string>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class PhysicalDevice {
public:
    PhysicalDevice(VkPhysicalDevice &physicalDevice);
    ~PhysicalDevice();

    std::vector<VkQueueFamilyProperties> getQueueFamilies() const;
    VkDevice createLogicalDevice(int graphicsFamily);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
    VkPhysicalDevice physicalDevice;
    int graphicsFamily;
    int displayFamily;

};


class Device {
public:
    Device(VkPhysicalDevice &physDevice);
    ~Device();
    PhysicalDevice &getPhysicalDevice();
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
    PhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    int graphicsFamily;
    int displayFamily;
};
