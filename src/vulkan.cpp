#include <iostream>
#include <vector>

#include "vulkan.h"


static void glfwError(int id, const char* description) {
    std::cout << "Error: " << description << std::endl;
}


VulkanInstance::VulkanInstance(bool enableValidation) {

    // must init glfw first
    glfwSetErrorCallback(&glfwError);
    if (!glfwInit()) {
        std::cout << "Initialization failed" << std::endl;
    }

    // check available extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    std::cout << extensionCount << " Instance Extensions supported:" << std::endl;
    for (const auto &extension: extensions) {
        std::cout << "\t" << extension.extensionName << std::endl;
    }

    // check required extensions
    std::vector<const char*> requiredExtensions;
    //requiredExtensions.push_back("VK_KHR_swapchain");
    requiredExtensions.push_back("VK_EXT_debug_report");
    unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::cout << glfwExtensionCount << " Extensions required:" << std::endl;
    for (int i = 0; i < glfwExtensionCount; ++i) {
        std::cout << "\t" << glfwExtensions[i] << std::endl;
        requiredExtensions.push_back(glfwExtensions[i]);
    }

    // check available validation layers
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    for (const auto &layerProperties: availableLayers) {
        std::cout << layerProperties.layerName << std::endl;
    }

    // add required validation layers
    std::vector<const char*> validationLayers;
    if (enableValidation) {
        validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
    }

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create instance" << std::endl;
    }
    else {
        std::cout << "Created instance" << std::endl;
    }
}


VulkanInstance::~VulkanInstance() {
    vkDestroyInstance(instance, nullptr);
    glfwTerminate();
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
