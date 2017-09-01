#pragma once

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "surface.h"


class SwapChain {
public:
    SwapChain(VkDevice &device, Surface &surface);
    ~SwapChain();

    size_t getFramebufferSize() const;
    VkSwapchainKHR &getSwapChain();
    VkFormat getImageFormat() const;
    VkExtent2D getExtent() const;
    VkFramebuffer &getFrameBuffer(size_t bufferIndex);

    void createFrameBuffers(VkDevice &device, VkRenderPass &renderPass);

private:
    VkDevice device;
    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

};
