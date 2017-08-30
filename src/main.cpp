#include <iostream>
#include <vector>

#include "device.h"
#include "surface.h"
#include "vulkan.h"
#include "window.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>


bool printDevice(VkPhysicalDevice &device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    VkPhysicalDeviceMemoryProperties deviceMemory;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    vkGetPhysicalDeviceMemoryProperties(device, &deviceMemory);
    bool discreteGpu = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    bool geometryShader = deviceFeatures.geometryShader;

    std::cout << "api version: " << deviceProperties.apiVersion << std::endl;
    std::cout << "driver version: " << deviceProperties.driverVersion << std::endl;
    std::cout << "selected device: " << deviceProperties.deviceName << std::endl;
    std::cout << "discrete gpu: " << discreteGpu << std::endl;
    std::cout << "geometry shader: " << geometryShader << std::endl;
    std::cout << "memory types: " << deviceMemory.memoryTypeCount << std::endl;
    std::cout << "memory heaps: " << deviceMemory.memoryHeapCount << std::endl;
    for (int i = 0; i < deviceMemory.memoryHeapCount; ++i) {
        std::cout << "size: " << deviceMemory.memoryHeaps[i].size << std::endl;
    }
}


int main() {
    VulkanInstance instance;

    // list devices
    std::vector<VkPhysicalDevice> devices;
    instance.enumeratePhysicalDevices(devices);
    for (auto &device: devices) {
        printDevice(device);
    }
    VkPhysicalDevice physicalDevice = devices[1];
    Device device(physicalDevice);

    // create window and surface
    Window window(instance.getVulkanInstance());
    Surface surface(physicalDevice, device.getGraphicsFamily(), window.getSurface());

    // create swap chain using window surface
    SwapChain swapChain(device.getVulkanDevice(), surface);




    Pipeline pipeline;



    //vertShaderModule = device.loadShaderModule("shaders/vert.spv");
    //fragShaderModule = device.loadShaderModule("shaders/frag.spv");

    while(!window.shouldClose()) {
        window.pollEvents();
    }
    return 0;
}
