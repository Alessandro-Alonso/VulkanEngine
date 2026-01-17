#pragma once
#include <vector>
#include <vulkan/vulkan.h>

// Este header contiene funciones especificas para Vulkan en macOS,
// como las extensiones necesarias para que funcione correctamente
// en esta plataforma.
namespace macOSVulkan {

    inline std::vector<const char*> getRequiredInstanceExtensions() {
        std::vector<const char*> extensions;

// Estas son las extensiones necesarias para que Vulkan funcione correctamente en macOS.
#ifdef __APPLE__
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#endif

        return extensions;
    }

    inline VkInstanceCreateFlags getInstanceCreateFlags() {

// Y esto es necesario para que Vulkan funcione correctamente en macOS,
// ya que esta plataforma no soporta todas las caracteristicas de Vulkan,
// y esta bandera le indica a Vulkan que debe enumerar los dispositivos
// de forma diferente para que funcione correctamente.
#ifdef __APPLE__
        return VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#else
        return 0;
#endif
    }

    inline std::vector<const char*> getRequiredDeviceExtensions() {
        std::vector<const char*> extensions;

// Esta es la extension necesaria para que Vulkan funcione correctamente en macOS,
// ya que esta plataforma no soporta todas las caracteristicas de Vulkan, y esta
// extension le indica a Vulkan que debe enumerar los dispositivos de forma diferente
// para que funcione correctamente.
#ifdef __APPLE__
        extensions.push_back("VK_KHR_portability_subset");
#endif

        return extensions;
    }

}