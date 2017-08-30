#pragma once

#include <string>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "pipeline.h"
#include "swapchain.h"


class Device {
public:
    Device(VkPhysicalDevice &physicalDevice);
    ~Device();
    VkDevice &getVulkanDevice();
    int getGraphicsFamily() const;
    void getPresentModes();
    VkQueue getGraphicsQueue();

    VkShaderModule loadShaderModule(const std::string &filepath);
    void createCommandPool(VkCommandPool &commandPool);

private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    int graphicsFamily;
    int displayFamily;
};
