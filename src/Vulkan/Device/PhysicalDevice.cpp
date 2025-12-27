#include "PhysicalDevice.h"
#include "Vulkan/Utils/VulkanTypes.h"
#include "Vulkan/Utils/ErrorHandling.h"
#include <vector>
#include <stdexcept>
#include <string>
#include <set>

namespace NETAEngine {

const std::vector<const char*> baseDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

void PhysicalDevice::pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) throw std::runtime_error("No GPUs support Vulkan.");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    int bestScore = 0;

    m_queueFamilyIndices = QueueFamilyIndices();

    for (const auto& vkDevice : devices) {

        QueueFamilyIndices indices = findQueueFamilies(vkDevice, surface);
        bool extensionsSupported = checkDeviceExtensionSupport(vkDevice, baseDeviceExtensions);

        if (!indices.isComplete() || !extensionsSupported) {
            continue;
        }

        int score = 0;
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(vkDevice, &props);

        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;
        score += props.limits.maxImageDimension2D;

        if (score > bestScore) {
            bestDevice = vkDevice;
            bestScore = score;
            m_physicalDevice = bestDevice;
            m_queueFamilyIndices = indices;
        }
    }

    if (bestScore > 0 && m_physicalDevice != VK_NULL_HANDLE) {
        queryDeviceCapabilities(m_physicalDevice);
    }
    else {
        throw std::runtime_error("No suitable GPU found.");
    }

}

bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface) {
    QueueFamilyIndices indices = findQueueFamilies(vkPhysicalDevice, surface);
    return indices.isComplete() && checkDeviceExtensionSupport(vkPhysicalDevice, baseDeviceExtensions);
}

void PhysicalDevice::queryDeviceCapabilities(VkPhysicalDevice vkPhysicalDevice) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

    bool hasMeshShaderExtension = false;
    for (const auto& ext : availableExtensions) {
        if (std::string(ext.extensionName) == VK_EXT_MESH_SHADER_EXTENSION_NAME) {
            hasMeshShaderExtension = true;
            break;
        }
    }

    VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES };
    VkPhysicalDeviceTimelineSemaphoreFeatures timelineFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES };
    VkPhysicalDeviceMeshShaderFeaturesEXT meshFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT };
    VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES };

    indexingFeatures.pNext = &timelineFeatures;
    timelineFeatures.pNext = &meshFeatures;
    meshFeatures.pNext = &dynamicRenderingFeatures;

    VkPhysicalDeviceFeatures2 features2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
    features2.pNext = &indexingFeatures;

    vkGetPhysicalDeviceFeatures2(vkPhysicalDevice, &features2);

    m_capabilities.samplerAnisotropy = features2.features.samplerAnisotropy;
    m_capabilities.fillModeNonSolid = features2.features.fillModeNonSolid;
    m_capabilities.timelineSemaphores = timelineFeatures.timelineSemaphore;
    m_capabilities.dynamicRendering = dynamicRenderingFeatures.dynamicRendering;

    m_capabilities.descriptorIndexing =
        indexingFeatures.runtimeDescriptorArray &&
        indexingFeatures.descriptorBindingPartiallyBound &&
        indexingFeatures.shaderSampledImageArrayNonUniformIndexing &&
        indexingFeatures.descriptorBindingVariableDescriptorCount;

    m_capabilities.meshShaders = hasMeshShaderExtension && meshFeatures.meshShader;
}

bool PhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice vkPhysicalDevice, const std::vector<const char*>& extensionsToCheck) {
    uint32_t count;
    vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> available(count);
    vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &count, available.data());

    std::set<std::string> required(extensionsToCheck.begin(), extensionsToCheck.end());
    for (const auto& ext : available) required.erase(ext.extensionName);
    return required.empty();
}

void PhysicalDevice::createLogicalDevice() {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        m_queueFamilyIndices.graphicsFamily.value(),
        m_queueFamilyIndices.presentFamily.value()
    };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures2 enabledFeatures2{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
    enabledFeatures2.features.samplerAnisotropy = m_capabilities.samplerAnisotropy;
    enabledFeatures2.features.fillModeNonSolid = m_capabilities.fillModeNonSolid;

    VkPhysicalDeviceDescriptorIndexingFeatures enabledIndexing{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES };
    VkPhysicalDeviceTimelineSemaphoreFeatures enabledTimeline{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES };
    VkPhysicalDeviceMeshShaderFeaturesEXT enabledMesh{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT };
    VkPhysicalDeviceDynamicRenderingFeatures enabledDynamicRendering{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES };

    void* currentPNext = nullptr;

    if (m_capabilities.dynamicRendering) {
        enabledDynamicRendering.dynamicRendering = VK_TRUE;
        enabledDynamicRendering.pNext = currentPNext;
        currentPNext = &enabledDynamicRendering;
    }
    else {
        throw std::runtime_error("GPU does not support Dynamic Rendering! To fucking old gpu.");
    }

    if (m_capabilities.descriptorIndexing) {
        enabledIndexing.runtimeDescriptorArray = VK_TRUE;
        enabledIndexing.descriptorBindingPartiallyBound = VK_TRUE;
        enabledIndexing.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        enabledIndexing.descriptorBindingVariableDescriptorCount = VK_TRUE;
        enabledIndexing.pNext = currentPNext;
        currentPNext = &enabledIndexing;
    }

    if (m_capabilities.timelineSemaphores) {
        enabledTimeline.timelineSemaphore = VK_TRUE;
        enabledTimeline.pNext = currentPNext;
        currentPNext = &enabledTimeline;
    }

    if (m_capabilities.meshShaders) {
        enabledMesh.meshShader = VK_TRUE;
        enabledMesh.taskShader = VK_TRUE;
        enabledMesh.pNext = currentPNext;
        currentPNext = &enabledMesh;
    }

    enabledFeatures2.pNext = currentPNext;

    std::vector<const char*> activeExtensions = baseDeviceExtensions;
    if (m_capabilities.meshShaders) activeExtensions.push_back(VK_EXT_MESH_SHADER_EXTENSION_NAME);

    // Portabilidad for Mac
    uint32_t extCount;
    vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extCount, nullptr);
    std::vector<VkExtensionProperties> availableExts(extCount);
    vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extCount, availableExts.data());

    for (const auto& ext : availableExts) {
        if (std::string(ext.extensionName) == "VK_KHR_portability_subset") {
            activeExtensions.push_back("VK_KHR_portability_subset");
            break;
        }
    }

    VkDeviceCreateInfo createInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pNext = &enabledFeatures2;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(activeExtensions.size());
    createInfo.ppEnabledExtensionNames = activeExtensions.data();

    VkDevice rawDevice = VK_NULL_HANDLE;
    VK_CHECK(vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &rawDevice));

    m_logicalDevice = DeviceHandle(rawDevice, DeviceDeleter{});

    vkGetDeviceQueue(m_logicalDevice.get(), m_queueFamilyIndices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(m_logicalDevice.get(), m_queueFamilyIndices.presentFamily.value(), 0, &presentQueue);
}

QueueFamilyIndices PhysicalDevice::findQueueFamilies(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface) {
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); ++i) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) indices.graphicsFamily = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, surface, &presentSupport);
        if (presentSupport) indices.presentFamily = i;

        if (indices.isComplete()) break;
    }
    return indices;
}
}