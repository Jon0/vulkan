#include <cstring>
#include <iostream>

#include "uniform.h"


Uniform::Uniform(Device &deviceObj)
    :
    instanceCount {8},
    device{deviceObj.getVulkanDevice()},
    uniformMemory {
        deviceObj.getPhysicalDevice(),
        deviceObj.getVulkanDevice(),
        sizeof(UniformBufferObject) * instanceCount,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    } {

    VkDescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    uboLayoutBinding.descriptorCount = instanceCount;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional


    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

}


Uniform::~Uniform() {
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}


VkBuffer &Uniform::getBuffer() {
    return uniformMemory.getBuffer();
}


std::vector<VkDescriptorBufferInfo> Uniform::getDescriptorBufferInfo() {
    std::vector<VkDescriptorBufferInfo> result;
    for (size_t i = 0; i < instanceCount; ++i) {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = uniformMemory.getBuffer();
        bufferInfo.offset = 0; // sizeof(UniformBufferObject) * i; // using dynamic offset
        bufferInfo.range = sizeof(UniformBufferObject);
        result.push_back(bufferInfo);
    }
    return result;
}


std::vector<VkDescriptorSetLayout> Uniform::getDescriptorSetLayout() {
    std::vector<VkDescriptorSetLayout> result;
    for (size_t i = 0; i < 1; ++i) {
        result.push_back(descriptorSetLayout);
    }
    return result;
}


void Uniform::updateUniformBuffer(const VkExtent2D &swapChainExtent) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    float aspectRatio = swapChainExtent.width / (float) swapChainExtent.height;
    UniformBufferObject ubo = {};

    for (size_t i = 0; i < instanceCount; ++i) {
        ubo.model = glm::rotate(glm::mat4(1.0f), (time * (i + 1)) * glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.view = glm::lookAt(glm::vec3(4.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10.0f);
        uniformMemory.copyDataTo(&ubo, sizeof(ubo) * i, sizeof(ubo));
    }
}
