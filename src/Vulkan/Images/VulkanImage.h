#pragma once

#include <vulkan/vulkan.h>
#include <Vulkan/Utils/vk_mem_alloc.h>

class VulkanImage {
public:
    VulkanImage(
        VmaAllocator allocator,
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageUsageFlags usage,
        VmaMemoryUsage vmaUsage
    );

    ~VulkanImage();

    VkImage getImage() const { return image; }

private:
    VmaAllocator allocator;
    VkImage image = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
};