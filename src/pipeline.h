#pragma once

#include <unordered_map>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.h"
#include "uniform.h"
#include "shader.h"


class PipelineBuilder {
public:
    void addShader(const VkShaderStageFlagBits type, const ShaderFile &file);

    void construct(Device &device);

private:
    std::unordered_map<VkShaderStageFlagBits, const ShaderFile> shaders;
    std::vector<Uniform> uniforms;


};


class Pipeline {
public:
    Pipeline(Device &device, const VkExtent2D &swapChainExtent, VkRenderPass &renderPass);
    ~Pipeline();
    VkPipeline &getVulkanPipeline();
    void addInitCommands(VkCommandBuffer &commandBuffer);
    void updateUniforms(const VkExtent2D &swapChainExtent);

private:
    void createLayout(VkDevice &device);
    void setup(VkDevice &device, const VkExtent2D &swapChainExtent, VkRenderPass &renderPass);

    VkDevice vulkanDevice;
    DescriptorPool descriptorPool;

    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;


};
