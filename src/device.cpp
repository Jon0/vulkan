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


void Device::createCommandPool(Pipeline &pipeline, SwapChain &swapChain) {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsFamily;
    poolInfo.flags = 0; // Optional
    VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create command pool" << std::endl;
    }

    std::vector<VkCommandBuffer> commandBuffers;
    commandBuffers.resize(swapChain.getFramebufferSize());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

    result = vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data());
    if (result != VK_SUCCESS) {
        std::cout << "Failed to allocate command buffers" << std::endl;
    }


    for (size_t i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        beginInfo.pInheritanceInfo = nullptr; // Optional

        vkBeginCommandBuffer(commandBuffers[i], &beginInfo);

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = swapChainFramebuffers[i];
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = swapChainExtent;
        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;
        vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
        vkCmdEndRenderPass(commandBuffers[i]);
        result = vkEndCommandBuffer(commandBuffers[i]);
        if (result != VK_SUCCESS) {
            std::cout << "Failed to record command buffer" << std::endl;
        }
    }

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create semaphores!");
    }
    result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create semaphores!");
    }
}


void Device::renderFrame(VkSwapchainKHR &swapChain) {
    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional
    vkQueuePresentKHR(graphicsQueue, &presentInfo);
}
