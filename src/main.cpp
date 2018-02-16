#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "device.h"
#include "renderpass.h"
#include "surface.h"
#include "swapchain.h"
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
    std::cout << "device name: " << deviceProperties.deviceName << std::endl;
    std::cout << "discrete gpu: " << discreteGpu << std::endl;
    std::cout << "geometry shader: " << geometryShader << std::endl;
    std::cout << "memory types: " << deviceMemory.memoryTypeCount << std::endl;
    std::cout << "memory heaps: " << deviceMemory.memoryHeapCount << std::endl;
    for (int i = 0; i < deviceMemory.memoryHeapCount; ++i) {
        double memSize = (double) deviceMemory.memoryHeaps[i].size / 1000000.0;
        std::cout << "size: " << memSize << " MB" << std::endl;
    }
}


int main(int argc, const char *argv[]) {

    // create vulkan instance with validation
    VulkanInstance instance(argc > 1);

    // list devices
    std::vector<VkPhysicalDevice> devices;
    instance.enumeratePhysicalDevices(devices);
    for (auto &device: devices) {
        printDevice(device);
    }
    VkPhysicalDevice physicalDevice = devices[0];
    Device device(physicalDevice);

    // create window and surface
    Window window(instance.getVulkanInstance(), 3200, 1800);
    Surface surface(physicalDevice, device.getGraphicsFamily(), window.getSurface());

    // create swap chain using window surface
    SwapChain swapChain(device.getVulkanDevice(), surface);
    RenderPass renderPass(physicalDevice, device.getVulkanDevice(), swapChain.getImageFormat());
    swapChain.createFrameBuffers(device.getVulkanDevice(), renderPass.getVulkanRenderPass());

    // setup pipeline
    Pipeline pipeline(device, swapChain.getExtent(), renderPass.getVulkanRenderPass());
    renderPass.initCommandPool(device, pipeline, swapChain);

    int frames = 0;
    auto start = std::chrono::system_clock::now();
    while(!window.shouldClose()) {
         window.pollEvents();
         renderPass.renderFrame(device, swapChain.getSwapChain());
    //     frames++;
         std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(1000));
    }

    // auto end = std::chrono::system_clock::now();
    // std::chrono::duration<double> diff = end - start;
    // double fps = (double) frames / diff.count();
    // std::cout << "rendered " << frames << " in " << diff.count() << " (" << fps << " fps)" << std::endl;

    return 0;
}
