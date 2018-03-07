#pragma once

#include <array>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "memory.h"
#include "pipeline.h"
#include "uniform.h"


struct Vertex {
    glm::vec3 pos;
    glm::vec3 colour;


    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }


    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, colour);
        return attributeDescriptions;
    }
};


const std::vector<Vertex> triangle = {
    {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
};


const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
};


const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};


/*
 * contains uniform data
 */
class GeometryInstance {
public:
    GeometryInstance();

private:

};


class GeometryBuffer {
public:
    GeometryBuffer(Device &deviceObj);
    ~GeometryBuffer();

    std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts();

    void copyData(Device &deviceObj);

    void setupDescriptorPool();

    void setupDescriptorSet();

    /*
     * submit drawing commands to queue
     */
    void render(VkCommandBuffer &commandBuffer, Pipeline &pipeline);

    void update(const VkExtent2D &swapChainExtent);

private:
    uint32_t bindingCount;
    uint32_t descriptorCount;
    VkDevice &device;
    VkDeviceSize vertexBufferSize;
    VkDeviceSize indexBufferSize;
    Memory vertexBuffer;
    Memory indexBuffer;
    Uniform uniformBuffer;

    // uniform bindings
    VkDescriptorPool descriptorPool;

    // should be per renderable instance
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<Vertex> verts;
    std::vector<uint16_t> inds;

};
