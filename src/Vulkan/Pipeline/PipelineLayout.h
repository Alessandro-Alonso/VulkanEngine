#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class PipelineLayout {
public:
    PipelineLayout(VkDevice device, const std::vector<VkDescriptorSetLayout>& descriptorLayouts);
    ~PipelineLayout();

    VkPipelineLayout get() const { return layout; }

private:
    VkDevice device;
    VkPipelineLayout layout;
};