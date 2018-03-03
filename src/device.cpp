#include <iostream>

#include "device.h"


void printQueueFamily(const VkQueueFamilyProperties &queueFamily) {
    std::cout << "queueFlags: " << queueFamily.queueFlags << std::endl;
    std::cout << "graphics bit: " << (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) << std::endl;
    std::cout << "queueCount: " << queueFamily.queueCount << std::endl;
    std::cout << "minImageTransferGranularity.width: " << queueFamily.minImageTransferGranularity.width << std::endl;
    std::cout << "minImageTransferGranularity.height: " << queueFamily.minImageTransferGranularity.height << std::endl;
    std::cout << "minImageTransferGranularity.depth: " << queueFamily.minImageTransferGranularity.depth << std::endl;
}


PhysicalDevice::PhysicalDevice(VkPhysicalDevice &physicalDevice)
    :
    physicalDevice {physicalDevice} {

}


PhysicalDevice::~PhysicalDevice() {

}


std::vector<VkQueueFamilyProperties> PhysicalDevice::getQueueFamilies() const {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
    return queueFamilies;
}


VkDevice PhysicalDevice::createLogicalDevice(int graphicsFamily) {
    float priorities[] = { 1.0f };
    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = NULL;
    queueInfo.flags = 0;
    queueInfo.queueFamilyIndex = graphicsFamily;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &priorities[0];



    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());
    std::cout << extensionCount << " Device Extensions supported:" << std::endl;
    for (const auto &extension: extensions) {
        std::cout << "\t" << extension.extensionName << std::endl;
    }

    std::vector<const char*> requiredExtensions;
    requiredExtensions.push_back("VK_KHR_swapchain");

    VkPhysicalDeviceFeatures deviceFeatures = {};
    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = NULL;
    deviceInfo.flags = 0;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pEnabledFeatures = &deviceFeatures;
    deviceInfo.enabledExtensionCount = requiredExtensions.size();
    deviceInfo.ppEnabledExtensionNames = requiredExtensions.data();

    VkDevice device;
    VkResult result = vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create logical device" << std::endl;
    }
    return device;
}


uint32_t PhysicalDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}



Device::Device(VkPhysicalDevice &physDevice)
    :
    physicalDevice {physDevice} {

    // list queue families
    std::vector<VkQueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilies();
    for (auto &queueFamily: queueFamilies) {
        printQueueFamily(queueFamily);
    }
    graphicsFamily = 0;

    device = physicalDevice.createLogicalDevice(graphicsFamily);
    vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
}


Device::~Device() {
    vkDeviceWaitIdle(device);
    vkDestroyDevice(device, nullptr);
}


PhysicalDevice &Device::getPhysicalDevice() {
    return physicalDevice;
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
