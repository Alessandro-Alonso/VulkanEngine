#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <type_traits>

namespace NETAEngine {

    struct DeviceDeleter {
        void operator()(VkDevice device) const {
            if (device != VK_NULL_HANDLE) vkDestroyDevice(device, nullptr);
        }
    };
    using DeviceHandle = std::unique_ptr<std::remove_pointer_t<VkDevice>, DeviceDeleter>;

    struct InstanceDeleter {
        void operator()(VkInstance instance) const {
            if (instance != VK_NULL_HANDLE) vkDestroyInstance(instance, nullptr);
        }
    };
    using InstanceHandle = std::unique_ptr<std::remove_pointer_t<VkInstance>, InstanceDeleter>;
}