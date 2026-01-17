#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

// La clase ShaderModule es un wrapper para un VkShaderModule, que se encarga de cargar el shader desde un archivo SPIR-V
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