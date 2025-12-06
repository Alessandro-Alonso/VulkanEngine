#pragma once

#include <vulkan/vulkan.h>

class PipelineLayout {
public:
    PipelineLayout(VkDevice device);
    ~PipelineLayout();

    VkPipelineLayout get() const { return layout; }

private:
    VkDevice device;
    VkPipelineLayout layout;
};