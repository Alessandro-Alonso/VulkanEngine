#include "VulkanImage.h"
#include "Vulkan\Utils\ErrorHandling.h"
#include "Vulkan\Utils\vk_mem_alloc.h"
#include <stdexcept>

VulkanImage::VulkanImage(
    VmaAllocator allocator,
    uint32_t width, uint32_t height, VkFormat format,
    VkImageUsageFlags usage,
    VmaMemoryUsage vmaUsage
) : allocator(allocator)
{
    // Crea la imagen
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.flags = 0;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = vmaUsage;

    // Error Handling Macro
    VK_CHECK(vmaCreateImage(allocator, &imageInfo, &allocInfo, &image, &allocation, nullptr));
}

VulkanImage::~VulkanImage() {
    if (image != VK_NULL_HANDLE)
        vmaDestroyImage(allocator, image, allocation);
}