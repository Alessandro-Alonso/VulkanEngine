#include "PhysicalDevice.h"
#include "Vulkan/Utils/VulkanTypes.h"
#include "Vulkan\Utils\ErrorHandling.h"
#include <vector>
#include <stdexcept>
#include <iostream>
#include <string>
#include <set>

// MoltenVk necesita esta extension para macOS
#ifdef __APPLE__
#ifndef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
#define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"
#endif
#endif

bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice vkPhysicalDevice) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &deviceFeatures);

#ifdef __APPLE__
    // Si el jugador esta en macOS, le decimos que MoltenVK no tiene soporte para la geometria de shaders. Lo saltamos.
    std::cout << "Running on macOS (MoltenVK). Using GPU:"
              << deviceProperties.deviceName << std::endl;
    return true; // Aceptar cualquier GPU. Me suda la puta polla, acepta todo y que deje de lloriquear el puto vulkan de mierda. Joder hostia puta ya.
#else
    // En Windows/Linux, necesitaremos una GPU que tenga soporte para geometry shaders.
    std::cout << "Running on Windows/Linux. Checking GPU:"
              << deviceProperties.deviceName << std::endl;
    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           deviceFeatures.geometryShader;
#endif
}

VkPhysicalDevice PhysicalDevice::pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("Error! You don't have any GPUs with vulkan support my guy. You can't play, sorry.");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    for (const auto& vkPhysicalDevice : devices) {  // Changed 'device' to 'vkPhysicalDevice' here too
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(vkPhysicalDevice, &props);
        if (isDeviceSuitable(vkPhysicalDevice)) {
            QueueFamilyIndices indices = findQueueFamilies(vkPhysicalDevice, surface);
            if (indices.isComplete()) {
                physicalDevice = vkPhysicalDevice;
                queueFamilyIndices = indices;
                return physicalDevice;
            }
        }
    }
    throw std::runtime_error("You don't have a GPU that supports presentation. IDK wtf this means either.");
};

QueueFamilyIndices PhysicalDevice::findQueueFamilies(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    for(uint32_t i = 0; i < queueFamilies.size(); i++) {
        const auto& queueFamily = queueFamilies[i];

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) break;
    }

    return indices;
}

void PhysicalDevice::createLogicalDevice(VkSurfaceKHR surface) {
    (void)surface;
    QueueFamilyIndices& indices = queueFamilyIndices;

    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.0f; 

    for(uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    // Extension del dispositivo
    std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    #ifdef __APPLE__
    deviceExtensions.push_back("VK_KHR_portability_subset");
    #endif

VkDeviceCreateInfo createInfo{};
createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
createInfo.pQueueCreateInfos = queueCreateInfos.data();
createInfo.pEnabledFeatures = &deviceFeatures;

createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
createInfo.ppEnabledExtensionNames = deviceExtensions.data();

// Layers de validacion
#ifdef ENABLE_VALIDATION_LAYERS
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
#else
    const std::vector<const char*> validationLayers = {};
#endif

    // Error handling.
    VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device));

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void PhysicalDevice::cleanup() {
    if (device != VK_NULL_HANDLE) {
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }
}