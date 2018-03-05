#include <iostream>

#include "vertex.h"

GeometryInstance::GeometryInstance() {

}



GeometryBuffer::GeometryBuffer(Device &deviceObj)
    :
    device {deviceObj.getVulkanDevice()},
    vertexBufferSize {sizeof(Vertex) * 1024},
    indexBufferSize {sizeof(uint16_t) * 1024},
    vertexBuffer {
        deviceObj.getPhysicalDevice(),
        device,
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    },
    indexBuffer {
        deviceObj.getPhysicalDevice(),
        device,
        indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    },
    uniformBuffer {deviceObj} {

    verts = vertices;
    inds = indices;
}


GeometryBuffer::~GeometryBuffer() {
    vkFreeDescriptorSets(device, descriptorPool, descriptorSets.size(), descriptorSets.data());
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}


std::vector<VkDescriptorSetLayout> GeometryBuffer::getDescriptorSetLayouts() {
    return uniformBuffer.getDescriptorSetLayout();
}


void GeometryBuffer::copyData(Device &deviceObj) {
    Memory vertexStagingBuffer {
        deviceObj.getPhysicalDevice(),
        deviceObj.getVulkanDevice(),
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };
    vertexStagingBuffer.copyData(verts.data());
    vertexBuffer.copyFromBuffer(deviceObj.getQueue(), vertexStagingBuffer);


    Memory indexStagingBuffer {
        deviceObj.getPhysicalDevice(),
        deviceObj.getVulkanDevice(),
        indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };
    indexStagingBuffer.copyData(inds.data());
    indexBuffer.copyFromBuffer(deviceObj.getQueue(), indexStagingBuffer);
}


void GeometryBuffer::setupDescriptorPool() {

    // must increase the limits here
    VkDescriptorPoolSize poolSize = {};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSize.descriptorCount = 8;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 8;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}


void GeometryBuffer::setupDescriptorSet() {

    std::vector<VkDescriptorBufferInfo> bufferInfo = uniformBuffer.getDescriptorBufferInfo();
    std::vector<VkDescriptorSetLayout> layoutInfo = uniformBuffer.getDescriptorSetLayout();


    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = layoutInfo.size();
    allocInfo.pSetLayouts = layoutInfo.data();

    descriptorSets.resize(layoutInfo.size());
    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor set!");
    }
    std::cout << descriptorSets.size() << std::endl;

    for (size_t i = 0; i < descriptorSets.size(); ++i) {
        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        descriptorWrite.descriptorCount = 8;
        descriptorWrite.pBufferInfo = bufferInfo.data();
        descriptorWrite.pImageInfo = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }


    // VkWriteDescriptorSet descriptorWrite = {};
    // descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    // descriptorWrite.dstSet = descriptorSets[0];
    // descriptorWrite.dstBinding = 0;
    // descriptorWrite.dstArrayElement = 0;
    // descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    // descriptorWrite.descriptorCount = bufferInfo.size();
    // descriptorWrite.pBufferInfo = bufferInfo.data();
    // descriptorWrite.pImageInfo = nullptr; // Optional
    // descriptorWrite.pTexelBufferView = nullptr; // Optional
    //
    // vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}


void GeometryBuffer::render(VkCommandBuffer &commandBuffer, Pipeline &pipeline) {
    std::vector<uint32_t> dynamicOffsets;
    for (uint32_t i = 0; i < 8; ++i) {
        dynamicOffsets.push_back(i * sizeof(UniformBufferObject));
    }

    VkBuffer vertexBuffers[] = {vertexBuffer.getBuffer()};
    VkDeviceSize offsets[] = {0};

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getVulkanPipeline());
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getLayout(), 0, descriptorSets.size(), descriptorSets.data(), dynamicOffsets.size(), dynamicOffsets.data());
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer.getBuffer(), 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 4, 0, 0, 0);

    //vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getLayout(), 0, 1, &descriptorSets[1], dynamicOffsets.size(), dynamicOffsets.data());
    //vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}


void GeometryBuffer::update(const VkExtent2D &swapChainExtent) {
    uniformBuffer.updateUniformBuffer(swapChainExtent);
}
