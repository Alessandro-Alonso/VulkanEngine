#include "ShaderModule.h"
#include "Vulkan\Utils\ErrorHandling.h"
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

    // Error Handling.
    VK_CHECK(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));
}

ShaderModule::~ShaderModule() {
    vkDestroyShaderModule(device, shaderModule, nullptr);
}

std::vector<char> ShaderModule::readFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    // Error handling.
    GE_CHECK(file.is_open(), "Failed to open shader file: " + filepath);

    size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    
    return buffer;
}