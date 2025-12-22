#include "RenderContext.h"
#include "Vulkan/Utils/ErrorHandling.h"
#include <stdexcept>

RenderContext::RenderContext(Window& win, VkInstance inst, VkSurfaceKHR surf)
    : window(win)
{
    physicalDeviceManager.pickPhysicalDevice(inst, surf);
    physicalDeviceManager.createLogicalDevice();

    // Esto es un tip para cuando lo veas en el futuro.
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

    queueFamilies = physicalDeviceManager.getQueueFamilyIndices();  // Guardamos copia

    swapChain = std::make_unique<VulkanSwapChain>(
        physicalDeviceManager.getPhysicalDevice(),
        physicalDeviceManager.getDevice(),
        surf,
        window.getWindow(),
        queueFamilies
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
    // 3. Destruye el dispositivo logico, lo ultimo.
    physicalDeviceManager.cleanup();
}