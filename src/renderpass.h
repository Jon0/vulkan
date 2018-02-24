#pragma once

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.h"
#include "memory.h"
#include "pipeline.h"
#include "swapchain.h"


class RenderPass {
public:
    RenderPass(Device &device, const VkFormat &swapChainImageFormat);
    ~RenderPass();

    VkRenderPass &getVulkanRenderPass();
    void initCommandPool(Device &device, Pipeline &pipeline, SwapChain &swapChain);
    void renderFrame(Device &device, VkSwapchainKHR &swapChain);

private:
    void allocateCommandBuffers(std::vector<VkCommandBuffer> &commandBuffers, VkDevice &device, size_t bufferCount);

    VkDevice device;
    VkRenderPass renderPass;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkFence> fences;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    Memory vertexData;

};
