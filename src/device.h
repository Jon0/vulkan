#pragma once

#include <string>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "pipeline.h"
#include "swapchain.h"


class Device {
public:
    Device(VkInstance &instance);
    ~Device();
    void getPresentModes();
    VkQueue getGraphicsQueue();
    VkShaderModule loadShaderModule(const std::string &filepath);
    void createCommandPool(Pipeline &pipeline, SwapChain &swapChain);
    void renderFrame(VkSwapchainKHR &swapChain);

private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    int graphicsFamily;
    int displayFamily;
    VkCommandPool commandPool;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
};
