#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

std::vector<char> readFile(const std::string& filename);

// Capas de validacion
extern const std::vector<const char*> validationLayers;
extern const bool enableValidationLayers;

// Declaracion de funciones (No estaticas)
bool checkValidationLayerSupport();
std::vector<const char*> getRequiredExtensions();

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator);