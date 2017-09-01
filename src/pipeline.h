#pragma once

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.h"


class Pipeline {
public:
    Pipeline(Device &device, const VkExtent2D &swapChainExtent, VkRenderPass &renderPass);
    ~Pipeline();
    VkPipeline &getVulkanPipeline();

private:
    void createLayout(VkDevice &device);
    void setup(VkDevice &device, const VkExtent2D &swapChainExtent, VkRenderPass &renderPass);

    VkDevice vulkanDevice;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

};
