#include "VulkanUtils.h"
#include "ErrorHandling.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <vector>

#include <GLFW/glfw3.h>

namespace NETAEngine {
    namespace Utils {

        // File IO
        std::vector<char> readFile(const std::string& filename) {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);

            //Error Handling.
            GE_CHECK(file.is_open(), "Failed to open file: " + filename);

            size_t fileSize = (size_t)file.tellg();

            // Tenemos que ver que la textura no pase de 4GB. Muchas Gpus estan puestas a 4GB, por los 32 bits de hardware. Incluso si tienes una 24gb RTX 4090 no te dejara.
            // Para que una imagen pese como minimo 4GB, tendria que ser 32,768 x 32,768 pixeles en un formato RGBA8, unos 4 bytes cada pixel. Es practicamente imposible que lo pases.
            if (fileSize > 100 * 1024 * 1024) {
                GE_CHECK(false, "File to large to load into memory buffer: " + filename);
            }

            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);
            file.close();

            return buffer;
        }

        // Support de validation layers
        bool checkValidationLayerSupport(const std::vector<const char*> & validationLayers) {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for (const char* layerName : validationLayers) {
                bool layerFound = false;

                for (const auto& layerProperties : availableLayers) {
                    if (strcmp(layerName, layerProperties.layerName) == 0) {
                        layerFound = true;
                        break;
                    }
                }

                if (!layerFound) {
                    return false;
                }
            }

            return true;
        }


        // Extensiones requeridas
        std::vector<const char*> getRequiredExtensions(bool enableValidationLayers) {
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

            if (enableValidationLayers) {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }

            return extensions;
        }

        // Callback de dbug
        VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            [[maybe_unused]] void* pUserData) {

            if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
                std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
            }
            return VK_FALSE;

        }

        // Funciones proxies para los mensajes de debug
        VkResult CreateDebugUtilsMessengerEXT(
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkDebugUtilsMessengerEXT* pDebugMessenger) { 

            auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

            if (func != nullptr) {
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            }
            else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        void DestroyDebugUtilsMessengerEXT(
            VkInstance instance,
            VkDebugUtilsMessengerEXT debugMessenger,
            const VkAllocationCallbacks* pAllocator) {
            
            auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

            if (func != nullptr) {
                func(instance, debugMessenger, pAllocator);
            }
        }

    }
}