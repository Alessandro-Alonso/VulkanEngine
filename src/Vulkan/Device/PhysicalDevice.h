#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include "Vulkan/Utils/VulkanTypes.h"

class PhysicalDevice {
public:
    VkPhysicalDevice pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
    VkPhysicalDevice getPhysicalDevice() const {return physicalDevice; }
    VkDevice getDevice() const { return device; }


    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }

    void createLogicalDevice(VkSurfaceKHR surface);
    void cleanup();

    QueueFamilyIndices getQueueFamilyIndices() const { return queueFamilyIndices; }

private:
    QueueFamilyIndices queueFamilyIndices;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    
    VkDevice device = VK_NULL_HANDLE;

    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    
    bool isDeviceSuitable(VkPhysicalDevice device);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
};