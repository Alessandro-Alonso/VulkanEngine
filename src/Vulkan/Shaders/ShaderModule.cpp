#include "ShaderModule.h"
#include <fstream>
#include <stdexcept>

ShaderModule::ShaderModule(VkDevice device, const std::string& filepath)
    : device(device)
{
    auto code = readFile(filepath);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        throw std::runtime_error("I can't create the shader module, im sorry: " + filepath);
}

ShaderModule::~ShaderModule() {
    vkDestroyShaderModule(device, shaderModule, nullptr);
}

std::vector<char> ShaderModule::readFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("My queen/king i can't open the shader file, im sorry: " + filepath);

    size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    
    return buffer;
}