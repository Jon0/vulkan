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


VkFormat PhysicalDevice::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (const VkFormat &format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}


VkFormat PhysicalDevice::findDepthFormat() {
    return findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
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


    commandQueue = std::make_unique<DeviceQueue>(device, graphicsFamily);
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


DeviceQueue &Device::getQueue() {
    return *commandQueue.get();
}


void Device::allocMemory(const VkMemoryRequirements &memRequirements, const VkMemoryPropertyFlags &properties, VkDeviceMemory &memory) {
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = physicalDevice.findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
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


DeviceQueue::DeviceQueue(VkDevice &device, int queueFamily)
    :
    device {device} {
    vkGetDeviceQueue(device, queueFamily, 0, &queue);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamily;
    poolInfo.flags = 0; // Optional

    VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create command pool" << std::endl;
    }
}


DeviceQueue::~DeviceQueue() {
    vkDestroyCommandPool(device, commandPool, nullptr);
}


void DeviceQueue::allocateCommandBuffers(VkCommandBuffer *commandBuffers, size_t bufferCount) {
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) bufferCount;

    VkResult result = vkAllocateCommandBuffers(device, &allocInfo, commandBuffers);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to allocate command buffers" << std::endl;
    }
}


void DeviceQueue::freeCommandBuffers(VkCommandBuffer *commandBuffers, size_t bufferCount) {
    vkFreeCommandBuffers(device, commandPool, bufferCount, commandBuffers);
}


void DeviceQueue::submit(VkSubmitInfo &submitInfo, const VkFence &fence) {
    if (vkQueueSubmit(queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        std::cout << "failed to submit draw command buffer!" << std::endl;
    }
}


void DeviceQueue::waitIdle() const {
    vkQueueWaitIdle(queue);
}
