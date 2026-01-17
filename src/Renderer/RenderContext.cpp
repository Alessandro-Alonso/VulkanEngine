#include "RenderContext.h"
#include "Vulkan/Utils/ErrorHandling.h"
#include "vulkan/vulkan_core.h"
#include "Vulkan/Device/PhysicalDevice.h"

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

        m_graphicsQueue = physicalDeviceManager.getGraphicsQueue();
        m_presentQueue = physicalDeviceManager.getPresentQueue();
    }

    RenderContext::~RenderContext() {
        if (allocator != VK_NULL_HANDLE) {
            vmaDestroyAllocator(allocator);
        }
    }

    VkQueue RenderContext::getGraphicsQueue() const { return m_graphicsQueue; }
    VkQueue RenderContext::getPresentQueue() const { return m_presentQueue; }

    void RenderContext::recreateSwapChain() {
        vkDeviceWaitIdle(getDevice());

        swapChain->recreate();
    }
}