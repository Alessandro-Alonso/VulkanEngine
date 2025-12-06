#pragma once

#include <vulkan/vulkan.h>
#include "Vulkan/Device/PhysicalDevice.h"
#include "Vulkan/Swapchain/VulkanSwapChain.h"
#include "Core/Window/Window.h"

class RenderContext {
public:
    RenderContext(Window& window, VkInstance instance, VkSurfaceKHR surface);
    ~RenderContext();

    // Getters
    VkDevice getDevice() const { return physicalDeviceManager.getDevice(); }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDeviceManager.getPhysicalDevice(); }
    VulkanSwapChain* getSwapChain() const { return swapChain; }

    QueueFamilyIndices getQueueFamilies() const { return physicalDeviceManager.getQueueFamilyIndices(); }

    void waitIdle() const { vkDeviceWaitIdle(getDevice()); }

private:
    Window& window;
    PhysicalDevice physicalDeviceManager;
    VulkanSwapChain* swapChain = nullptr;
    QueueFamilyIndices queueFamilies;

    friend class HelloTriangleApplication;
};