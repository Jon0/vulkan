#include <cstring>
#include <iostream>
#include <fstream>

#include "device.h"


bool printDevice(VkPhysicalDevice &device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    bool discreteGpu = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    bool geometryShader = deviceFeatures.geometryShader;

    std::cout << "selected device: " << deviceProperties.deviceName << std::endl;
    std::cout << "discrete gpu: " << discreteGpu << std::endl;
    std::cout << "geometry shader: " << geometryShader << std::endl;
}


VkPhysicalDevice pickPhysicalDevice(VkInstance &instance) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    if (devices.size() > 0) {
        return devices[0];
    }
}


Device::Device(VkInstance &instance) {
    VkPhysicalDevice physicalDevice = pickPhysicalDevice(instance);
    printDevice(physicalDevice);

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
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
    //deviceInfo.enabledExtensionCount = glfwExtensionCount;
    //deviceInfo.ppEnabledExtensionNames = glfwExtensions;
    deviceInfo.pEnabledFeatures = &deviceFeatures;

    VkResult result = vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create logical device" << std::endl;
    }
}


Device::~Device() {
    vkDeviceWaitIdle(device);
}


VkDevice &Device::getVulkanDevice() {
    return device;
}


VkQueue Device::getGraphicsQueue() {
    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
    return graphicsQueue;
}


VkShaderModule Device::loadShaderModule(const std::string &filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        std::cout << "failed to open file!" << std::endl;
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
