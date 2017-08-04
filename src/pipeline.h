#pragma once

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class Pipeline {
public:
    Pipeline();
    VkInstance &getInstance();
    void createLayout(VkDevice &device);
    void createRenderPass(VkDevice &device, const VkFormat &swapChainImageFormat);
    void loadShaders(VkShaderModule &vertShader, VkShaderModule &fragShader);
    void setup(VkDevice &device, const VkExtent2D &swapChainExtent);

private:
    VkInstance instance;
    VkRenderPass renderPass;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

};
