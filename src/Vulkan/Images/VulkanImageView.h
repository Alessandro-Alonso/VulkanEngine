#pragma once

#include <vulkan/vulkan.h>

class VulkanImageView {
public:
    VulkanImageView(
        VkDevice device,
        VkImage image,
        VkFormat format,
        VkImageAspectFlags aspectFlags
    );

    ~VulkanImageView();

    VkImageView getView() const { return view; }

private:
    VkDevice device;
    VkImageView view = VK_NULL_HANDLE;
};