#pragma once

#include <array>
#include <unordered_map>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/epsilon.hpp>

#include "memory.h"
#include "pipeline.h"
#include "uniform.h"


struct Vertex {
    glm::vec4 pos;
    glm::vec3 texCoord;
    glm::vec3 normal;


    bool operator==(const Vertex& other) const {
        float epsilon = 0.00001f;
        auto p = glm::epsilonEqual(pos, other.pos, glm::vec4(epsilon));
        auto t = glm::epsilonEqual(texCoord, other.texCoord, glm::vec3(epsilon));
        auto n = glm::epsilonEqual(normal, other.normal, glm::vec3(epsilon));
        return p.x && p.y && p.z && p.w && t.x && t.y && t.z && n.x && n.y && n.z;
    }


    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }


    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, texCoord);
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, normal);
        return attributeDescriptions;
    }
};


namespace std {
    template<> struct hash<Vertex>
    {
        typedef Vertex argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& v) const noexcept {
            result_type const h1 ( std::hash<glm::vec4>{}(v.pos) );
            result_type const h2 ( std::hash<glm::vec3>{}(v.texCoord) );
            result_type const h3 ( std::hash<glm::vec3>{}(v.normal) );
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}


class GeometryBuilder {
public:
    void addPolygon(const std::vector<Vertex> &verts);
    size_t addVertex(const Vertex &vert);

    void outputVerts(std::vector<Vertex> &verts) const;
    void outputInds(std::vector<uint16_t> &inds) const;

private:
    std::vector<Vertex> data;
    std::unordered_map<Vertex, size_t> index;
    std::vector<std::vector<size_t>> polygons;

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
    GeometryBuffer(Device &deviceObj, const GeometryBuilder &builder);
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
