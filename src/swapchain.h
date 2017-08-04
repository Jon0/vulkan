#pragma once

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "surface.h"


class SwapChain {
public:
    SwapChain(VkDevice &device, Surface &surface);
    size_t getFramebufferSize() const;
    VkSwapchainKHR getSwapChain() const;
    VkExtent2D getExtent() const;
    void createFrameBuffers(VkDevice &device, VkRenderPass &renderPass);

private:
    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

};
