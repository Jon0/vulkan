#pragma once

#include <array>
#include <string>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


class ShaderFile {
public:
    ShaderFile(const std::string &filepath);
    ~ShaderFile();
    bool createModule(VkDevice &device, VkShaderModule &shaderModule) const;

private:
    size_t fileSize;
    std::vector<uint32_t> codeAligned;

};
