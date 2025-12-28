#include "RenderContext.h"
#include "Vulkan/Utils/ErrorHandling.h"

namespace NETAEngine {

    RenderContext::RenderContext(Window& win, VkInstance inst, VkSurfaceKHR surf)
        : window(win), m_surface(surf)
    {
        physicalDeviceManager.pickPhysicalDevice(inst, surf);
        physicalDeviceManager.createLogicalDevice();

        // Siempre pasa los punteros de funcion de forma explicita.
        // Esto hace que VMA pueda trabajar si usas despues un
        // loader dinamico.
        VmaVulkanFunctions vulkanFunctions = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        allocatorInfo.physicalDevice = physicalDeviceManager.getPhysicalDevice();
        allocatorInfo.device = physicalDeviceManager.getDevice();
        allocatorInfo.instance = inst;
        allocatorInfo.pVulkanFunctions = &vulkanFunctions;

        VK_CHECK(vmaCreateAllocator(&allocatorInfo, &allocator));

        swapChain = std::make_unique<VulkanSwapChain>(
            physicalDeviceManager.getPhysicalDevice(),
            physicalDeviceManager.getDevice(),
            surf,
            window.getWindow(),
            physicalDeviceManager.getQueueFamilyIndices()
        );
    }

    RenderContext::~RenderContext() {
        // 1. Primero rompe el swapchain (Usa el dispositivo)
        if (swapChain) {
            swapChain->cleanup();
        }
        // 2. Destruye el VMA despues. (Usa el dispositivo)
        if (allocator != VK_NULL_HANDLE) {
            vmaDestroyAllocator(allocator);
        }
    }

    VkQueue RenderContext::getGraphicsQueue() const {
        VkQueue queue;
        vkGetDeviceQueue(getDevice(), getQueueFamilies().graphicsFamily.value(), 0, &queue);
        return queue;
    }

    VkQueue RenderContext::getPresentQueue() const {
        VkQueue queue;
        vkGetDeviceQueue(getDevice(), getQueueFamilies().presentFamily.value(), 0, &queue);
        return queue;
    }

    void RenderContext::recreateSwapChain() {
        vkDeviceWaitIdle(getDevice());

        swapChain.reset();

        swapChain = std::make_unique<VulkanSwapChain>(
            physicalDeviceManager.getPhysicalDevice(),
            physicalDeviceManager.getDevice(),
            m_surface,
            window.getWindow(),
            physicalDeviceManager.getQueueFamilyIndices()
        );
    }
}