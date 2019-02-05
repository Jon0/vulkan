#pragma once

#include "memory.h"


class Image {
public:
    Image(Device &dev, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

    VkImageView createImageView(VkFormat format, VkImageAspectFlags aspectFlags);

    void resizeTo(uint32_t width, uint32_t height);

private:
    Device &dev;
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkMemoryPropertyFlags properties;
    
    VkImage image;
    VkMemoryRequirements memRequirements;
    VkDeviceMemory imageMemory;
};
