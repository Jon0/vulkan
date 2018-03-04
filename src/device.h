#pragma once

#include <memory>
#include <string>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class DeviceQueue {
public:
    DeviceQueue(VkDevice &device, int queueFamily);
    ~DeviceQueue();
    void allocateCommandBuffers(VkCommandBuffer *commandBuffers, size_t bufferCount);
    void freeCommandBuffers(VkCommandBuffer *commandBuffers, size_t bufferCount);
    void submit(VkSubmitInfo &submitInfo, const VkFence &fence);
    void waitIdle() const;

private:
    VkDevice &device;
    VkQueue queue;
    VkCommandPool commandPool;

};


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

    /*
     * returns initialised commandPool
     */
    DeviceQueue &getQueue();


    /*
     * construct vulkan semaphore
     */
    void createSemaphore(VkSemaphore &semaphore);

private:
    PhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    int graphicsFamily;
    int displayFamily;
    std::unique_ptr<DeviceQueue> commandQueue;

};
