#include <iostream>
#include <vector>

#include "vulkan.h"


static void glfwError(int id, const char* description) {
    std::cout << "Error: " << description << std::endl;
}


VulkanInstance::VulkanInstance() {

    // must init glfw first
    glfwSetErrorCallback(&glfwError);
    if (!glfwInit()) {
        std::cout << "Initialization failed" << std::endl;
    }

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    std::cout << extensionCount << " Extensions supported:" << std::endl;
    for (const auto &extension: extensions) {
        std::cout << "\t" << extension.extensionName << std::endl;
    }

    unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::cout << glfwExtensionCount << " Extensions required:" << std::endl;

    for (int i = 0; i < glfwExtensionCount; ++i) {
        std::cout << "\t" << glfwExtensions[i] << std::endl;
    }

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create instance" << std::endl;
    }
}


VulkanInstance::~VulkanInstance() {
    vkDestroyInstance(instance, nullptr);
}


VkInstance &VulkanInstance::getVulkanInstance() {
    return instance;
}


void VulkanInstance::enumeratePhysicalDevices(std::vector<VkPhysicalDevice> &devices) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    devices.resize(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
}
