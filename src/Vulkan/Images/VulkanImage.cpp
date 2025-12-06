#include "VulkanImage.h"
#include <stdexcept>

VulkanImage::VulkanImage(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties
) : device(device)
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

    if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
        throw std::runtime_error("Something happened and i can't create the Vulkan Image. Do some research. Or curse to whoever made this piece of shit code. ");

    // Asignar memoria    
    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(device, image, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = findMemoryType(
        physicalDevice,
        memReq.memoryTypeBits,
        properties
    );

    if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
        throw std::runtime_error("For some reason i can't allocate the image memory. I don't know why.");

    vkBindImageMemory(device, image, memory, 0);

}    

VulkanImage::~VulkanImage() {
    if (image != VK_NULL_HANDLE) vkDestroyImage(device, image, nullptr);
    if (memory != VK_NULL_HANDLE) vkFreeMemory(device, memory, nullptr);
}

uint32_t VulkanImage::findMemoryType(
    VkPhysicalDevice physicalDevice,
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties
) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
        (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        return i;
    }

    throw std::runtime_error("I can't find a suitable memory type! im sorry.");
}