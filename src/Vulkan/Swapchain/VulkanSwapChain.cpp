#include "VulkanSwapChain.h"
#include "Vulkan/Utils/ErrorHandling.h"
#include "vulkan/vulkan_core.h"
#include <iostream>
#include <stdexcept>

namespace NETAEngine {

    // El constructor de VulkanSwapChain recibe el dispositivo fisico, el dispositivo logico, la superficie, la ventana y los indices de las colas,
    // y se encarga de crear el swap chain y las image views, que son necesarias para renderizar en la ventana.
    VulkanSwapChain::VulkanSwapChain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, GLFWwindow* window, const QueueFamilyIndices& indices)
        : physicalDevice(physicalDevice), device(device), surface(surface), window(window), queueIndices(indices) {
        createSwapChain();
        createImageViews();
    }

    // Sencillo, un cleanup de los recursos.
    VulkanSwapChain::~VulkanSwapChain() { 
        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
        }
        swapChainImageViews.clear();

        if (swapChain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(device, swapChain, nullptr);
            swapChain = VK_NULL_HANDLE;
        }
    }

    // Esto se encarga de limpiar los recursos del swap chain, como las image views y el swap chain en si,
    // lo cual es necesario para recrear el swap chain cuando se redimensiona la ventana, o cuando se-----
    // pierde el swap chain por cualquier motivo.
    void VulkanSwapChain::cleanup() {
    
        for (auto view : swapChainImageViews) {
            vkDestroyImageView(device, view, nullptr);
        }
        swapChainImageViews.clear();
    
        if (swapChain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(device, swapChain, nullptr);
            swapChain = VK_NULL_HANDLE;
        }

    }

    // Primero se obtiene la informacion del swap chain, como las capacidades, los formatos y los present modes,
    // luego se elige el formato de superficie, el present mode y la extension del swap chain, y finalmente se crea el swap chain con esa informacion,
    // lo cual es necesario para renderizar en la ventana, ya que el swap chain es el encargado de presentar las imagenes renderizadas en la ventana.
    SwapChainSupportDetails VulkanSwapChain::querySwapChainSupport(VkPhysicalDevice newdevice) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(newdevice, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(newdevice, surface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(newdevice, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(newdevice, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(newdevice, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    // Esto funciona de la siguiente manera: se recibe una lista de formatos disponibles, y se elige el formato que se prefiere, que en este caso es VK_FORMAT_B8G8R8A8_SRGB
    // con el espacio de color VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, que es el formato mas comunmente soportado por las tarjetas graficas.
    VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    // Lo que hace esto es elegir el present mode que se prefiere, que en este caso es VK_PRESENT_MODE_MAILBOX_KHR, que es el present mode mas rapido y con menos latencia,
    // pero si no esta disponible, se elige VK_PRESENT_MODE_FIFO_KHR, que es el present mode mas comunmente soportado por las tarjetas graficas.
    VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    // Esto funciona de la siguiente manera: se recibe la informacion de las capacidades del swap chain, y se elige la extension del swap chain que se prefiere,
    // que en este caso es la extension de la ventana, que es el tamaño de la ventana, pero si la extension de la ventana no esta disponible, se elige la extension minima.
    VkExtent2D VulkanSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    // Esto se encarga de crear el swap chain con la informacion obtenida anteriormente, y luego se obtienen las imagenes del swap chain,
    // que son las imagenes que se van a renderizar en la ventana, y se guardan en un vector para poder acceder a ellas posteriormente.
    void VulkanSwapChain::createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);



        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkImageUsageFlags desiredUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                                    | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                                    | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

        VkImageUsageFlags supportedUsage = swapChainSupport.capabilities.supportedUsageFlags;
        VkImageUsageFlags finalUsage     = desiredUsage & supportedUsage;

        if(!(finalUsage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)) {
            throw std::runtime_error("Surface does not support VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT. Cannot render. ");
        }

        if (!(finalUsage & VK_IMAGE_USAGE_TRANSFER_DST_BIT)) {
            std::cerr << "[Warning] VK_IMAGE_USAGE_TRANSFER_DST_BIT not supported. Blit-to-swapchain unavailable." << std::endl;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface          = surface;
        createInfo.minImageCount    = imageCount;
        createInfo.imageFormat      = surfaceFormat.format;
        createInfo.imageColorSpace  = surfaceFormat.colorSpace;
        createInfo.imageExtent      = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = finalUsage;
        createInfo.preTransform     = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode      = presentMode;
        createInfo.clipped          = VK_TRUE;
        createInfo.oldSwapchain     = swapChain;


        uint32_t queueFamilyIndices[] = {
            queueIndices.graphicsFamily.value(),
            queueIndices.presentFamily.value()
        };

        if (queueIndices.graphicsFamily != queueIndices.presentFamily) {
            createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices   = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        VkSwapchainKHR newSwapChain = VK_NULL_HANDLE;
        VK_CHECK(vkCreateSwapchainKHR(device, &createInfo, nullptr, &newSwapChain));

        // Destruir recursos viejos despues del nuevo swapchain esta confirmad que esta vivo.
        if (swapChain != VK_NULL_HANDLE) {
            for (auto imageView : swapChainImageViews) {
                vkDestroyImageView(device, imageView, nullptr);
            }
            swapChainImageViews.clear();
            vkDestroySwapchainKHR(device, swapChain, nullptr);
        }

        swapChain = newSwapChain;

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent      = extent;
    }

    void VulkanSwapChain::recreate() {
        createSwapChain();
        createImageViews();
    }

    // Esto se encarga de crear las image view para cada imagen del swap chain.
    void VulkanSwapChain::createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {

            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            // Error Handling.
            VK_CHECK(vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]));
        }
    }

}

