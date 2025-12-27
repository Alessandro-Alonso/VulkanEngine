#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Vulkan/Utils/VulkanTypes.h"
#include "Vulkan/Utils/VulkanRAII.h"

namespace NETAEngine {

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
        PhysicalDevice() = default;
        ~PhysicalDevice() = default;

        PhysicalDevice(const PhysicalDevice&) = delete;
        PhysicalDevice& operator=(const PhysicalDevice&) = delete;

        PhysicalDevice(PhysicalDevice&&) noexcept = default;
        PhysicalDevice& operator=(PhysicalDevice&&) noexcept = default;

        void pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
        void createLogicalDevice();

        VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
        VkDevice getDevice() const { return m_logicalDevice.get(); }

        const DeviceCapabilities& getCapabilities() const { return m_capabilities; }
        QueueFamilyIndices getQueueFamilyIndices() const { return m_queueFamilyIndices; }

    private:
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        DeviceHandle m_logicalDevice;

        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;
        QueueFamilyIndices m_queueFamilyIndices;
        DeviceCapabilities m_capabilities;

        bool isDeviceSuitable(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface);
        bool checkDeviceExtensionSupport(VkPhysicalDevice vkPhysicalDevice, const std::vector<const char*>& requiredExtensions);
        void queryDeviceCapabilities(VkPhysicalDevice vkPhysicalDevice);
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface);
    };
}