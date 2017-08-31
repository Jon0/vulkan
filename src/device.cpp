#include <cstring>
#include <iostream>
#include <fstream>

#include "device.h"


void printQueueFamily(const VkQueueFamilyProperties &queueFamily) {
    std::cout << "queueFlags: " << queueFamily.queueFlags << std::endl;
    std::cout << "graphics bit: " << (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) << std::endl;
    std::cout << "queueCount: " << queueFamily.queueCount << std::endl;
    std::cout << "minImageTransferGranularity.width: " << queueFamily.minImageTransferGranularity.width << std::endl;
    std::cout << "minImageTransferGranularity.height: " << queueFamily.minImageTransferGranularity.height << std::endl;
    std::cout << "minImageTransferGranularity.depth: " << queueFamily.minImageTransferGranularity.depth << std::endl;
}


Device::Device(VkPhysicalDevice &physDevice)
    :
    physicalDevice {physDevice} {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
    for (auto &queueFamily: queueFamilies) {
            printQueueFamily(queueFamily);
    }
    graphicsFamily = 0;

    float priorities[] = { 1.0f };
    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = NULL;
    queueInfo.flags = 0;
    queueInfo.queueFamilyIndex = graphicsFamily;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &priorities[0];

    VkPhysicalDeviceFeatures deviceFeatures = {};
    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = NULL;
    deviceInfo.flags = 0;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pEnabledFeatures = &deviceFeatures;
    //deviceInfo.enabledExtensionCount = glfwExtensionCount;
    //deviceInfo.ppEnabledExtensionNames = glfwExtensions;

    VkResult result = vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create logical device" << std::endl;
    }
    vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
}


Device::~Device() {
    vkDeviceWaitIdle(device);
    vkDestroyDevice(device, nullptr);
}


VkDevice &Device::getVulkanDevice() {
    return device;
}


VkQueue &Device::getGraphicsQueue() {
    return graphicsQueue;
}


int Device::getGraphicsFamily() const {
    return graphicsFamily;
}


VkShaderModule Device::loadShaderModule(const std::string &filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        std::cout << "failed to open file " << filepath << std::endl;
    }


    size_t fileSize = (size_t) file.tellg();
    std::vector<char> code(fileSize);
    file.seekg(0);
    file.read(code.data(), fileSize);


    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();

    std::vector<uint32_t> codeAligned(code.size() / sizeof(uint32_t) + 1);
    memcpy(codeAligned.data(), code.data(), code.size());
    createInfo.pCode = codeAligned.data();

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        std::cout << "Failed to create shader module" << std::endl;
    }
    return shaderModule;
}


void Device::createCommandPool(VkCommandPool &commandPool) {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsFamily;
    poolInfo.flags = 0; // Optional
    VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create command pool" << std::endl;
    }
}


void Device::createSemaphore(VkSemaphore &semaphore) {
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore);
    if (result != VK_SUCCESS) {
        std::cout << "failed to create semaphore!" << std::endl;
    }
}
