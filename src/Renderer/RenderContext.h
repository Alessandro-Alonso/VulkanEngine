#pragma once

#include <vulkan/vulkan.h>
#include "Vulkan/Device/PhysicalDevice.h"
#include "Vulkan/Swapchain/VulkanSwapChain.h"
#include "Vulkan/Utils/vk_mem_alloc.h"
#include "Core/Window/Window.h"
#include <sstream>

class RenderContext {
public:
    RenderContext(Window& window, VkInstance instance, VkSurfaceKHR surface);
    ~RenderContext();

    // Getters
    VkDevice getDevice() const { return physicalDeviceManager.getDevice(); }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDeviceManager.getPhysicalDevice(); }
    VulkanSwapChain* getSwapChain() const { return swapChain.get(); }
    VmaAllocator getAllocator() const { return allocator; }

    QueueFamilyIndices getQueueFamilies() const { return physicalDeviceManager.getQueueFamilyIndices(); }

    void waitIdle() const { vkDeviceWaitIdle(getDevice()); }

private:
    Window& window;
    PhysicalDevice physicalDeviceManager;
    std::unique_ptr<VulkanSwapChain> swapChain;
    QueueFamilyIndices queueFamilies;
    VmaAllocator allocator;

    friend class HelloTriangleApplication;
};