#pragma once

#include "memory.h"


class Image {
public:
    Image(Device &dev, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

    VkImageView createImageView(VkFormat format, VkImageAspectFlags aspectFlags);

private:
    VkDevice &device;
    VkImage image;
    VkMemoryRequirements memRequirements;
    VkDeviceMemory imageMemory;
};
