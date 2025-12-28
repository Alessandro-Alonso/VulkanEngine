#pragma once

#include <vulkan/vulkan.h>
#include <optional>

#include "vk_mem_alloc.h"

namespace NETAEngine {

    struct AllocatedBuffer {
        VkBuffer _buffer = VK_NULL_HANDLE;
        VmaAllocation _allocation = VK_NULL_HANDLE;
        VmaAllocator _allocator = VK_NULL_HANDLE;

        AllocatedBuffer() = default;

        AllocatedBuffer(AllocatedBuffer&& other) noexcept {
            _buffer = other._buffer;
            _allocation = other._allocation;
            _allocator = other._allocator;

            other._buffer = VK_NULL_HANDLE;
            other._allocation = VK_NULL_HANDLE;
        }

        ~AllocatedBuffer() {
            if (_buffer != VK_NULL_HANDLE && _allocator != VK_NULL_HANDLE) {
                vmaDestroyBuffer(_allocator, _buffer, _allocation);
            }
        }

        AllocatedBuffer(const AllocatedBuffer&) = delete;
        AllocatedBuffer& operator=(const AllocatedBuffer&) = delete;
    };

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };
}