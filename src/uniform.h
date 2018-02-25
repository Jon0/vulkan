#pragma once

#include <array>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include "memory.h"
#include "device.h"

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};


class Uniform {
public:
    Uniform(Device &deviceObj);
    ~Uniform();

    void createUniformBuffer();
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    VkBuffer &getBuffer();
    VkDescriptorSetLayout &getDescriptorSetLayout();
    void updateUniformBuffer(const VkExtent2D &swapChainExtent);

private:
    Memory uniformMemory;
    VkDevice &device;
    VkDescriptorSetLayout descriptorSetLayout;

};
