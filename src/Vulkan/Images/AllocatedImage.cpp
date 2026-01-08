#include "AllocatedImage.h"
#include "Vulkan/Utils/ErrorHandling.h"

namespace NETAEngine {

    AllocatedImage::AllocatedImage(VkDevice device, VmaAllocator allocator,
        VkExtent3D extent, VkFormat format,
        VkImageUsageFlags usage, VkImageAspectFlags aspectFlags)
        : device(device), allocator(allocator), extent(extent), format(format)
    {
        // Creamos la imagen
        VkImageCreateInfo imgInfo{};
        imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imgInfo.imageType = VK_IMAGE_TYPE_2D;
        imgInfo.format = format;
        imgInfo.extent = extent;
        imgInfo.mipLevels = 1;
        imgInfo.arrayLayers = 1;
        imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imgInfo.usage = usage;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VK_CHECK(vmaCreateImage(allocator, &imgInfo, &allocInfo, &image, &allocation, nullptr));

        // Creamos el image view
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.image = image;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &imageView));
    }

    AllocatedImage::~AllocatedImage() {
        cleanup();
    }

    AllocatedImage::AllocatedImage(AllocatedImage&& other) noexcept {
        device = other.device;
        allocator = other.allocator;
        image = other.image;
        imageView = other.imageView;
        allocation = other.allocation;
        extent = other.extent;
        format = other.format;

        other.image = VK_NULL_HANDLE;
        other.imageView = VK_NULL_HANDLE;
        other.allocation = VK_NULL_HANDLE;
    }

    AllocatedImage& AllocatedImage::operator=(AllocatedImage&& other) noexcept {
        if (this != &other) {
            cleanup();

            device = other.device;
            allocator = other.allocator;
            image = other.image;
            imageView = other.imageView;
            allocation = other.allocation;
            extent = other.extent;
            format = other.format;

            other.image = VK_NULL_HANDLE;
            other.imageView = VK_NULL_HANDLE;
            other.allocation = VK_NULL_HANDLE;
        }
        return *this;
    }

    void AllocatedImage::cleanup() {
        if (imageView != VK_NULL_HANDLE) {
            vkDestroyImageView(device, imageView, nullptr);
            imageView = VK_NULL_HANDLE;
        }
        if (image != VK_NULL_HANDLE) {
            vmaDestroyImage(allocator, image, allocation);
            image = VK_NULL_HANDLE;
            allocation = VK_NULL_HANDLE;
        }
    }
}