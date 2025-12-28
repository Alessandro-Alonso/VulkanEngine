#pragma once

#include "Core/Window/Window.h"
#include "Renderer/Renderer.h"

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace NETAEngine {

    class HelloTriangleApplication {
    public:
        HelloTriangleApplication();
        ~HelloTriangleApplication();

        HelloTriangleApplication(const HelloTriangleApplication&) = delete;
        HelloTriangleApplication& operator=(const HelloTriangleApplication&) = delete;

        void run();
    private:
        void initVulkan();
        void mainLoop();

        // Contextos de ayuda de Vulkan
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        // Systemas centrales
        Window m_window;
        std::unique_ptr<Renderer> m_renderer;

        // Handles de vulkan
        VkInstance m_instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;

        // Configuracion layers de validacion. Para debugging, te dice los problemas, errores o fallos que tienes basicamente. En la version release, se quita.
        // ESTO BAJA UN 50% LOS FPS. RECUERDA.
        #ifdef NDEBUG
                const bool m_enableValidationLayers = false;
        #else
                const bool m_enableValidationLayers = true;
        #endif

        const std::vector<const char*> m_validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };
    };
}