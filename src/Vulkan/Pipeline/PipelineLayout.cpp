#include "PipelineLayout.h"
#include "Vulkan\Utils\ErrorHandling.h"
#include <stdexcept>

PipelineLayout::PipelineLayout(VkDevice device, const std::vector<VkDescriptorSetLayout>& descriptorLayouts)
    : device(device)
{
    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    layoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorLayouts.size());
    layoutInfo.pSetLayouts = descriptorLayouts.data();

    layoutInfo.pushConstantRangeCount = 0;
    layoutInfo.pPushConstantRanges = nullptr;

    // Error Handling.
    VK_CHECK(vkCreatePipelineLayout(device, &layoutInfo, nullptr, &layout));
}

PipelineLayout::~PipelineLayout() {
    vkDestroyPipelineLayout(device, layout, nullptr);
}