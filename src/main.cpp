#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "device.h"
#include "model.h"
#include "renderpass.h"
#include "surface.h"
#include "swapchain.h"
#include "vulkan.h"
#include "window.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
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
    return true;
}


int main(int argc, const char *argv[]) {
    std::string objFilePath = "assets/Box.obj";
    bool enableValidation = false;
    
    for (size_t i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--validate") {
            enableValidation = true;
        }
        else if (arg == "--obj") {
            objFilePath = argv[++i];
            std::cout << "Using obj file: " << objFilePath << std::endl;
        }
    }


    // create vulkan instance with validation
    VulkanInstance instance(enableValidation);

    // list devices
    std::vector<VkPhysicalDevice> devices;
    instance.enumeratePhysicalDevices(devices);
    for (auto &device: devices) {
        printDevice(device);
    }
    VkPhysicalDevice physicalDevice = devices[0];
    Device device(physicalDevice);

    // create window and surface
    Window window(instance.getVulkanInstance(), 800, 600);
    Surface surface(physicalDevice, device.getGraphicsFamily(), window.getSurface());

    // create swap chain using window surface
    SwapChain swapChain(device, surface);

    // render pass is a pipeline component
    RenderPass renderPass(device, swapChain.getImageFormat());
    swapChain.createFrameBuffers(renderPass.getVulkanRenderPass());

    // setup geometry
    ObjFile obj(objFilePath);
    GeometryBuffer geometry(device, obj.getBuilder());
    geometry.setupDescriptorPool();
    geometry.setupDescriptorSet();
    geometry.copyData(device);

    // new pipeline setup
    PipelineBuilder pipelineBuilder;
    pipelineBuilder.addShader(VK_SHADER_STAGE_VERTEX_BIT, ShaderFile("shaders/vert.spv"));
    pipelineBuilder.addShader(VK_SHADER_STAGE_FRAGMENT_BIT, ShaderFile("shaders/frag.spv"));
    pipelineBuilder.addDescriptorSetLayouts(geometry.getDescriptorSetLayouts());
    pipelineBuilder.setOutputImageExtent(swapChain.getExtent());
    pipelineBuilder.setRenderPass(renderPass.getVulkanRenderPass());

    // setup pipeline
    Pipeline pipeline(device.getVulkanDevice());
    pipelineBuilder.construct(pipeline);

    renderPass.initCommandPool(device, geometry, pipeline, swapChain);

    int frames = 0;
    auto start = std::chrono::system_clock::now();
    while(!window.shouldClose()) {
        window.pollEvents();
        geometry.update(swapChain.getExtent());
        renderPass.renderFrame(device.getGraphicsQueue(), swapChain.getSwapChain());
        std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(5));
        frames++;
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    double fps = (double) frames / diff.count();
    std::cout << "rendered " << frames << " frames in " << diff.count() << " seconds (" << fps << " fps)" << std::endl;

    return 0;
}
