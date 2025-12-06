#include "RenderContext.h"
#include <stdexcept>

RenderContext::RenderContext(Window& win, VkInstance inst, VkSurfaceKHR surf)
    : window(win)
{
    physicalDeviceManager.pickPhysicalDevice(inst, surf);
    physicalDeviceManager.createLogicalDevice(surf);

    queueFamilies = physicalDeviceManager.getQueueFamilyIndices();  // Guardamos copia

    swapChain = new VulkanSwapChain(
        physicalDeviceManager.getPhysicalDevice(),
        physicalDeviceManager.getDevice(),
        surf,
        window.getWindow(),
        queueFamilies  // Usamos la copia
    );
}

RenderContext::~RenderContext() {
    if (swapChain) {
        swapChain->cleanup();
        delete swapChain;
    }
    physicalDeviceManager.cleanup();
}