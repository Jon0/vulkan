#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.h"
#include "shader.h"
#include "uniform.h"


class Pipeline {
public:
    Pipeline(VkDevice &device);
    ~Pipeline();

    VkPipelineLayout &getLayout();
    VkPipeline &getVulkanPipeline();
    VkPipeline &getDescriptorPool();

    void setupShaders(const std::unordered_map<VkShaderStageFlagBits, const ShaderFile> &shaders);
    void setupLayout(const std::vector<VkDescriptorSetLayout> &layouts);
    void setupPipeline(const VkExtent2D &swapChainExtent, VkRenderPass &renderPass);

private:
    VkDevice vulkanDevice;

    std::unordered_map<VkShaderStageFlagBits, VkShaderModule> shaderModules;

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
};


class PipelineBuilder {
public:
    void addShader(const VkShaderStageFlagBits type, const ShaderFile &file);
    void addDescriptorSetLayouts(const std::vector<VkDescriptorSetLayout> &layouts);
    void setInputVertexFormat();
    void setOutputImageExtent(const VkExtent2D &extent);
    void setRenderPass(const VkRenderPass &pass);

    void construct(Pipeline &pipeline);

private:
    std::unordered_map<VkShaderStageFlagBits, const ShaderFile> shaders;
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    VkExtent2D outputExtent;
    VkFormat outputFormat;
    VkRenderPass renderPass;

};
