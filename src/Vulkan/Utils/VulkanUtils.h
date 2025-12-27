#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace NETAEngine {
    namespace Utils { 

    // Ayuda para leer los archivos binarios.
    std::vector<char> readFile(const std::string& filename);

    // Mira si los layeres especificos son soportados por los drivers de la grafica.
    bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);

    // Coje las extensiones requeridas por GLFW + Los layeres de validacion (si estan activados).
    std::vector<const char*> getRequiredExtensions(bool enableValidationLayers);

    // Debugcallback para los layeres de validacion
    VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    //  Funciones proxy para cargar los puntos de extensiones
    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator);

    }
}