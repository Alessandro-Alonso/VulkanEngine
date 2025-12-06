#pragma once
#include <vulkan/vulkan.h>
#include "Core/Window/Window.h"
#include "Renderer/Renderer.h"

class HelloTriangleApplication {
public:
    void run();

private:
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();

    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    Window window;
    Renderer* renderer = nullptr;

    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    const bool enableValidationLayers = true;
};