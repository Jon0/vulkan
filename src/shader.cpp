#include <cstring>
#include <iostream>
#include <fstream>

#include "shader.h"


ShaderFile::ShaderFile(const std::string &filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        std::cout << "failed to open file " << filepath << std::endl;
    }

    fileSize = (size_t) file.tellg();
    std::vector<char> code(fileSize);
    file.seekg(0);
    file.read(code.data(), fileSize);

    std::vector<uint32_t> codeAligned(code.size() / sizeof(uint32_t) + 1);
    memcpy(codeAligned.data(), code.data(), code.size());
}


ShaderFile::~ShaderFile() {}


bool ShaderFile::createModule(VkDevice &device, VkShaderModule &shaderModule) {

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = fileSize;
    createInfo.pCode = codeAligned.data();

    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        std::cout << "Failed to create shader module" << std::endl;
        return false;
    }
    return true;
}
