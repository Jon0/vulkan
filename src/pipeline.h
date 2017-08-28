#pragma once

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class Pipeline {
public:
    Pipeline();
    VkInstance &getInstance();
    VkPipeline &getVulkanPipeline();
    void createLayout(VkDevice &device);
    void loadShaders(VkShaderModule &vertShader, VkShaderModule &fragShader);
    void setup(VkDevice &device, const VkExtent2D &swapChainExtent, VkRenderPass &renderPass);

private:
    VkInstance instance;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

};
