#pragma once

#include <vulkan/vulkan.h>

class GraphicsPipeline {
public:
    GraphicsPipeline(
        VkDevice device,
        VkExtent2D swapchainExtent,
        VkRenderPass renderPass,
        VkPipelineLayout pipelineLayout,
        const char* vertFile,
        const char* fragFile
    );

    ~GraphicsPipeline();

    VkPipeline get() const { return pipeline; }

private:
    VkDevice device;
    VkPipeline pipeline;
};