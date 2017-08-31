#pragma once

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class VulkanInstance {
public:
    VulkanInstance(bool enableValidation);
    ~VulkanInstance();

    VkInstance &getVulkanInstance();
    void enumeratePhysicalDevices(std::vector<VkPhysicalDevice> &devices);

private:
    VkInstance instance;

};
