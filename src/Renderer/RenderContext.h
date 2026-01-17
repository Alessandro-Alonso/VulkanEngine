#pragma once

#include "Vulkan/Device/PhysicalDevice.h"
#include "Vulkan/Swapchain/VulkanSwapChain.h"

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
#include "Core/Window/Window.h"

#include <memory>

namespace NETAEngine {

    class RenderContext {
    public:
        RenderContext(Window& window, VkInstance instance, VkSurfaceKHR surface);
        ~RenderContext();
   
        VkDevice getDevice() const { return physicalDeviceManager.getDevice(); }
        VkPhysicalDevice getPhysicalDevice() const { return physicalDeviceManager.getPhysicalDevice(); }
        VulkanSwapChain* getSwapChain() const { return swapChain.get(); }

        QueueFamilyIndices getQueueFamilies() const { return physicalDeviceManager.getQueueFamilyIndices(); }

        VkQueue getGraphicsQueue() const;
        VkQueue getPresentQueue() const;
        VkQueue m_graphicsQueue = VK_NULL_HANDLE;
        VkQueue m_presentQueue = VK_NULL_HANDLE;

        void waitIdle() const { vkDeviceWaitIdle(getDevice()); }
        void recreateSwapChain();
        
        VmaAllocator getAllocator() const { return allocator; }

    private:
        Window& window;
        PhysicalDevice physicalDeviceManager;
        std::unique_ptr<VulkanSwapChain> swapChain;
        VmaAllocator allocator;

        VkSurfaceKHR m_surface;
    };

}