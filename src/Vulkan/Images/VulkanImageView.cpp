#include "VulkanImageView.h"
#include <stdexcept>

VulkanImageView::VulkanImageView(
    VkDevice device,
    VkImage image,
    VkFormat format,
    VkImageAspectFlags aspectFlags
) : device(device)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;

    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &viewInfo, nullptr, &view) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create the Vulkan ImageView. If this shit comes up, the developer did something wrong. Contact him and insult him.");
    }
}

VulkanImageView::~VulkanImageView() {
    if (view != VK_NULL_HANDLE) {
        vkDestroyImageView(device, view, nullptr);
    }
}
