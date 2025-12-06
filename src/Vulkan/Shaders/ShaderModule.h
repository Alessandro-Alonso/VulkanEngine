#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class ShaderModule {
public:
    ShaderModule(VkDevice device, const std::string& filepath);
    ~ShaderModule();

    VkShaderModule get() const { return shaderModule; }

private:
    VkDevice device;
    VkShaderModule shaderModule;

    static std::vector<char> readFile(const std::string& filepath);
};