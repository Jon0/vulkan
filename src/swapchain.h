#pragma once

#include <array>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "image.h"
#include "surface.h"


class SwapChain {
public:
    SwapChain(Device &device, Surface &surface);
    ~SwapChain();

    size_t getFramebufferSize() const;
    VkSwapchainKHR &getSwapChain();
    VkFormat getImageFormat() const;
    VkExtent2D getExtent() const;
    VkFramebuffer &getFrameBuffer(size_t bufferIndex);

    void createFrameBuffers(VkRenderPass &renderPass);

private:
    VkDevice &device;
    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    // depth buffer
    VkFormat depthImageFormat;
    Image depthImage;

    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

};
