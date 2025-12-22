#include "PipelineLayout.h"
#include "Vulkan\Utils\ErrorHandling.h"
#include <stdexcept>

PipelineLayout::PipelineLayout(VkDevice device)
    : device(device)
{
    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    // Error Handling.
    VK_CHECK(vkCreatePipelineLayout(device, &layoutInfo, nullptr, &layout));
}

PipelineLayout::~PipelineLayout() {
    vkDestroyPipelineLayout(device, layout, nullptr);
}