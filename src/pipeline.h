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

    VkPipeline &getVulkanPipeline();
    void addInitCommands(VkCommandBuffer &commandBuffer);
    void updateUniforms(const VkExtent2D &swapChainExtent);

    void setupShaders(const std::unordered_map<VkShaderStageFlagBits, const ShaderFile> &shaders);
    void setupDescriptorPool(const std::vector<std::shared_ptr<Uniform>> &uniformData);
    void setupLayout();
    void setupPipeline(const VkExtent2D &swapChainExtent, VkRenderPass &renderPass);

private:
    VkDevice vulkanDevice;

    std::unordered_map<VkShaderStageFlagBits, VkShaderModule> shaderModules;
    std::vector<std::shared_ptr<Uniform>> uniforms;

    // uniforms
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
};


class PipelineBuilder {
public:
    void addShader(const VkShaderStageFlagBits type, const ShaderFile &file);
    void addUniform(const std::shared_ptr<Uniform> &uniform);
    void setInputVertexFormat();
    void setOutputImageExtent(const VkExtent2D &extent);
    void setRenderPass(const VkRenderPass &pass);

    void construct(Pipeline &pipeline);

private:
    std::unordered_map<VkShaderStageFlagBits, const ShaderFile> shaders;
    std::vector<std::shared_ptr<Uniform>> uniforms;
    VkExtent2D outputExtent;
    VkFormat outputFormat;
    VkRenderPass renderPass;

};
