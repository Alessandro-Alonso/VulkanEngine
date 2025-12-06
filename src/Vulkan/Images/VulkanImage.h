#pragma once
#include <vulkan/vulkan.h>

class VulkanImage {
public:
    VulkanImage(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties
    );

    ~VulkanImage();

    VkImage getImage() const { return image; }
    VkDeviceMemory getMemory() const { return memory; }

private:
    VkDevice device;

    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    uint32_t findMemoryType(
        VkPhysicalDevice physicalDevice,
        uint32_t typeFilter,
        VkMemoryPropertyFlags properties
    );
};