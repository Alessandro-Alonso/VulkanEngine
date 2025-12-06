#include "PipelineLayout.h"
#include <stdexcept>

PipelineLayout::PipelineLayout(VkDevice device)
    : device(device)
{
    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    if(vkCreatePipelineLayout(device, &layoutInfo, nullptr, &layout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create PipelineLayout");
}

PipelineLayout::~PipelineLayout() {
    vkDestroyPipelineLayout(device, layout, nullptr);
}