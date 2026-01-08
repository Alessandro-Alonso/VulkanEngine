#pragma once

#include <vulkan/vulkan.h>
#include "Vulkan/Shaders/ShaderModule.h"

class GraphicsPipeline {
public:
    GraphicsPipeline(
        VkDevice device,
        VkExtent2D swapchainExtent,
        VkFormat colorFormat,
        VkFormat depthFormat,
        VkPipelineLayout layout,
        const char* vertFile,
        const char* fragFile
    );

    ~GraphicsPipeline();

    VkPipeline get() const { return pipeline; }

private:
    VkDevice device;
    VkPipeline pipeline = VK_NULL_HANDLE;

    ShaderModule vertexShaderModule;
    ShaderModule fragmentShaderModule;
};