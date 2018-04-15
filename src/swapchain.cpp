#include <iostream>

#include "swapchain.h"

SwapChain::SwapChain(Device &dev, Surface &surface)
    :
    device {dev.getVulkanDevice()},
    swapChainExtent {surface.getExtent()},
    depthImageFormat {dev.getPhysicalDevice().findDepthFormat()},
    depthImage {
        dev,
        swapChainExtent.width,
        swapChainExtent.height,
        depthImageFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    } {

    VkSurfaceCapabilitiesKHR capabilities = surface.getCapabilities();
    VkSurfaceFormatKHR surfaceFormat = surface.getSurfaceFormat();
    uint32_t imageCount = surface.getImageCount();

    VkSwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface               = surface.getSurface();
    swapchainInfo.minImageCount         = imageCount;
    swapchainInfo.imageFormat           = surfaceFormat.format;
    swapchainInfo.imageColorSpace       = surfaceFormat.colorSpace;
    swapchainInfo.imageExtent           = swapChainExtent;
    swapchainInfo.imageArrayLayers      = 1;
    swapchainInfo.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.queueFamilyIndexCount = 0;       // Optional
    swapchainInfo.pQueueFamilyIndices   = nullptr; // Optional
    swapchainInfo.preTransform          = capabilities.currentTransform;
    swapchainInfo.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode           = surface.getPresentMode();
    swapchainInfo.clipped               = VK_TRUE;
    swapchainInfo.oldSwapchain          = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &swapChain);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create swap chain" << std::endl;
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
    swapChainImageFormat = surfaceFormat.format;

    swapChainImageViews.resize(swapChainImages.size());
    for (uint32_t i = 0; i < swapChainImages.size(); ++i) {
        VkImageViewCreateInfo imageViewInfo = {};
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.image = swapChainImages[i];
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = swapChainImageFormat;
        imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.levelCount = 1;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(device, &imageViewInfo, nullptr, &swapChainImageViews[i]);
        if (result != VK_SUCCESS) {
            std::cout << "Failed to create image view" << std::endl;
        }
    }
}


SwapChain::~SwapChain() {
    for (size_t i = 0; i < swapChainFramebuffers.size(); ++i) {
        vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
    }
    for (size_t i = 0; i < swapChainImages.size(); ++i) {
        vkDestroyImageView(device, swapChainImageViews[i], nullptr);
    }
    vkDestroySwapchainKHR(device, swapChain, nullptr);
}


size_t SwapChain::getFramebufferSize() const {
    return swapChainFramebuffers.size();
}


VkSwapchainKHR &SwapChain::getSwapChain() {
    return swapChain;
}


VkFormat SwapChain::getImageFormat() const {
    return swapChainImageFormat;
}


VkExtent2D SwapChain::getExtent() const {
    return swapChainExtent;
}


VkFramebuffer &SwapChain::getFrameBuffer(size_t bufferIndex) {
    return swapChainFramebuffers[bufferIndex];
}


void SwapChain::createFrameBuffers(VkRenderPass &renderPass) {
    VkImageView depthImageView = depthImage.createImageView(depthImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    swapChainFramebuffers.resize(swapChainImageViews.size());
    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        std::array<VkImageView, 2> attachments = {
            swapChainImageViews[i],
            depthImageView
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]);
        if (result != VK_SUCCESS) {
            std::cout << "Failed to create framebuffer" << std::endl;
        }
    }
}
