#pragma once

#include <vulkan/vulkan.h>
#include "Vulkan/Utils/vk_mem_alloc.h"

namespace NETAEngine {

	class AllocatedImage {
		public:
            AllocatedImage(VkDevice device, VmaAllocator allocator,
                VkExtent3D extent, VkFormat format,
                VkImageUsageFlags usage, VkImageAspectFlags aspectFlags);

            ~AllocatedImage();

            AllocatedImage(const AllocatedImage&) = delete;
            AllocatedImage& operator=(const AllocatedImage&) = delete;

            AllocatedImage(AllocatedImage&& other) noexcept;
            AllocatedImage& operator=(AllocatedImage&& other) noexcept;

            VkImage getImage() const { return image; }
            VkImageView getImageView() const { return imageView; }
            VkFormat getFormat() const { return format; }
            VkExtent3D getExtent() const { return extent; }

     private:
         void cleanup();

         VkDevice device = VK_NULL_HANDLE;
         VmaAllocator allocator = VK_NULL_HANDLE;

         VkImage image = VK_NULL_HANDLE;
         VkImageView imageView = VK_NULL_HANDLE;
         VmaAllocation allocation = VK_NULL_HANDLE;

         VkExtent3D extent{};
         VkFormat format = VK_FORMAT_UNDEFINED;
    };

}