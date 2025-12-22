#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>
#include <string>
#include "Vulkan/Utils/VulkanTypes.h"

struct DeviceCapabilities {
    bool samplerAnisotropy = false;
    bool fillModeNonSolid = false;
    bool descriptorIndexing = false;
    bool timelineSemaphores = false;
    bool dynamicRendering = false;
    bool meshShaders = false;
    bool portabilitySubset = false;
};

class PhysicalDevice {
public:
    void pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
    void createLogicalDevice();
    void cleanup();

    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    VkDevice getDevice() const { return logicalDevice; }
    const DeviceCapabilities& getCapabilities() const { return capabilities; }
    QueueFamilyIndices getQueueFamilyIndices() const { return queueFamilyIndices; }

private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    QueueFamilyIndices queueFamilyIndices;
    DeviceCapabilities capabilities;

    bool isDeviceSuitable(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface);
    bool checkDeviceExtensionSupport(VkPhysicalDevice vkPhysicalDevice, const std::vector<const char*>& requiredExtensions);
    void queryDeviceCapabilities(VkPhysicalDevice vkPhysicalDevice);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface);
};