#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class VulkanInstance {
public:
    VulkanInstance();

private:
    VkInstance instance;

};
