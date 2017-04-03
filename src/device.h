#pragma once


class Device {
public:
    Device(VkInstance &instance);


    VkShaderModule loadShaderModule(const std::string &filepath);


private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    int graphicsFamily;
};
