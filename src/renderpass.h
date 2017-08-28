#pragma once

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.h"


class RenderPass {
public:
    RenderPass(VkDevice &device, const VkFormat &swapChainImageFormat);
    void initCommandPool(Device &device, Pipeline &pipeline, SwapChain &swapChain);
    void renderFrame(VkDevice &device, VkSwapchainKHR &swapChain);

private:
    void allocateCommandBuffers(std::vector<VkCommandBuffer> &commandBuffers, VkDevice &device, size_t bufferCount);

    VkRenderPass renderPass;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;

};
