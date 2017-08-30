#include <iostream>

#include "surface.h"


Surface::Surface(VkPhysicalDevice &physicalDevice, int graphicsFamily, VkSurfaceKHR &windowSurface) {
    surface = windowSurface;
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsFamily, surface, &presentSupport);
    std::cout << "Present support: " << presentSupport << std::endl;


    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
    extent = capabilities.currentExtent;
    std::cout << "extent: " << extent.width << ", " << extent.height << std::endl;

    imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }
    std::cout << "image count: " << imageCount << std::endl;

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());
    }

    std::cout << "format count: " << formatCount << std::endl;
    for (const auto fmt: formats) {
        if (fmt.format == VK_FORMAT_B8G8R8A8_UNORM) {
            surfaceFormat = fmt;
        }
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
    }

    std::cout << "present mode count: " << presentModeCount << std::endl;
    for (const auto p: presentModes) {
        if (p == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = p;
        }
    }
}


VkSurfaceKHR Surface::getSurface() const {
    return surface;
}


VkSurfaceCapabilitiesKHR Surface::getCapabilities() const {
    return capabilities;
}


VkSurfaceFormatKHR Surface::getSurfaceFormat() const {
    return surfaceFormat;
}


VkPresentModeKHR Surface::getPresentMode() const {
    return presentMode;
}


VkExtent2D Surface::getExtent() const {
    return extent;
}


uint32_t Surface::getImageCount() const {
    return imageCount;
}
