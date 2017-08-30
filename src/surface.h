#pragma once

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class Surface {
public:
    Surface(VkPhysicalDevice &physicalDevice, int graphicsFamily, VkSurfaceKHR &windowSurface);
    VkSurfaceKHR getSurface() const;
    VkSurfaceCapabilitiesKHR getCapabilities() const;
    VkSurfaceFormatKHR getSurfaceFormat() const;
    VkPresentModeKHR getPresentMode() const;
    VkExtent2D getExtent() const;
    uint32_t getImageCount() const;


private:
    VkSurfaceKHR surface;
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    VkExtent2D extent;
    uint32_t imageCount;

};
