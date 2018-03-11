#include <iostream>

#include <glm/ext.hpp>

#include "vertex.h"


void GeometryBuilder::addPolygon(const std::vector<Vertex> &verts) {
    std::vector<size_t> inds;
    for (const auto &v : verts) {
        inds.push_back(addVertex(v));
    }
    polygons.push_back(inds);
}


size_t GeometryBuilder::addVertex(const Vertex &vert) {
    auto v = index.find(vert);
    if (v == index.end()) {
        data.push_back(vert);
        size_t new_index = data.size() - 1;
        index.insert(std::make_pair(vert, new_index));
        return new_index;
    }
    else {
        return v->second;
    }
}


void GeometryBuilder::outputVerts(std::vector<Vertex> &verts) const {
    verts = data;
}


void GeometryBuilder::outputInds(std::vector<uint16_t> &inds) const {
    for (const auto &p : polygons) {
        for (const auto &i : p) {
            inds.push_back(i);
        }
    }
}


GeometryInstance::GeometryInstance() {

}



GeometryBuffer::GeometryBuffer(Device &deviceObj, const GeometryBuilder &builder)
    :
    bindingCount {1},
    descriptorCount {1},
    device {deviceObj.getVulkanDevice()},
    vertexBufferSize {sizeof(Vertex) * 1024},
    indexBufferSize {sizeof(uint16_t) * 1024 * 8},
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
    uniformBuffer {deviceObj, descriptorCount} {


    builder.outputVerts(verts);
    builder.outputInds(inds);
    //verts = vertices;
    //inds = indices;


    std::cout << verts.size() << std::endl;
    std::cout << inds.size() << std::endl;
}


GeometryBuffer::~GeometryBuffer() {
    vkFreeDescriptorSets(device, descriptorPool, descriptorSets.size(), descriptorSets.data());
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}


std::vector<VkDescriptorSetLayout> GeometryBuffer::getDescriptorSetLayouts() {
    return std::vector<VkDescriptorSetLayout> {
        uniformBuffer.getDescriptorSetLayout()
    };
}


void GeometryBuffer::copyData(Device &deviceObj) {
    Memory vertexStagingBuffer {
        deviceObj.getPhysicalDevice(),
        deviceObj.getVulkanDevice(),
        vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    // unsafe copy
    vertexStagingBuffer.copyData(verts.data(), verts.size() * sizeof(Vertex));
    vertexBuffer.copyFromBuffer(deviceObj.getQueue(), vertexStagingBuffer);


    Memory indexStagingBuffer {
        deviceObj.getPhysicalDevice(),
        deviceObj.getVulkanDevice(),
        indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };
    indexStagingBuffer.copyData(inds.data(), inds.size() * sizeof(uint16_t));
    indexBuffer.copyFromBuffer(deviceObj.getQueue(), indexStagingBuffer);
}


void GeometryBuffer::setupDescriptorPool() {

    // must increase the limits here
    VkDescriptorPoolSize poolSize = {};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = descriptorCount;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = descriptorCount;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}


void GeometryBuffer::setupDescriptorSet() {

    VkDescriptorSetLayout layoutInfo = uniformBuffer.getDescriptorSetLayout();
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    for (uint32_t i = 0; i < descriptorCount; ++i) {
        descriptorSetLayouts.push_back(layoutInfo);
    }

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = descriptorSetLayouts.size();
    allocInfo.pSetLayouts = descriptorSetLayouts.data();

    descriptorSets.resize(descriptorSetLayouts.size());
    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor set!");
    }

    for (size_t i = 0; i < descriptorSets.size(); ++i) {
        VkDescriptorBufferInfo bufferInfo = uniformBuffer.getDescriptorBufferInfo(i);
        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }
}


void GeometryBuffer::render(VkCommandBuffer &commandBuffer, Pipeline &pipeline) {
    VkBuffer vertexBuffers[] = {vertexBuffer.getBuffer()};
    VkDeviceSize offsets[] = {0};

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getVulkanPipeline());
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer.getBuffer(), 0, VK_INDEX_TYPE_UINT16);


    for (auto &descriptorSet : descriptorSets) {
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getLayout(), 0, 1, &descriptorSet, 0, nullptr);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(inds.size()), 1, 0, 0, 0);
    }
}


void GeometryBuffer::update(const VkExtent2D &swapChainExtent) {
    uniformBuffer.updateUniformBuffer(swapChainExtent);
}
