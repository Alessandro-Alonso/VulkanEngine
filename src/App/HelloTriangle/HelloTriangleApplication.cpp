#include "HelloTriangleApplication.h"
#include "Vulkan/Utils/VulkanUtils.h"
#include <stdexcept>
#include <iostream>

void HelloTriangleApplication::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void HelloTriangleApplication::initWindow() {
    window.initWindow();
}

void HelloTriangleApplication::initVulkan() {
    createInstance();
    setupDebugMessenger();
    createSurface();

    renderer = new Renderer(window, instance, surface);
}

void HelloTriangleApplication::mainLoop() {
    while (!glfwWindowShouldClose(window.getWindow())) {
        glfwPollEvents();
        renderer->drawFrame();
    }
    renderer->waitIdle();
}

void HelloTriangleApplication::cleanup() {
    delete renderer;

    if (enableValidationLayers && debugMessenger != VK_NULL_HANDLE) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    if (surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }

    vkDestroyInstance(instance, nullptr);

    window.cleanup();
}

void HelloTriangleApplication::createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("Im asking for the Validation layers, but you don't have them, mister.");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef __APPLE__
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("I don't fucking know what happened, but i can't create the vulkan instance. Sorry.");
    }
}

void HelloTriangleApplication::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;
}

void HelloTriangleApplication::setupDebugMessenger() {
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("I can't set up the debug messenger, idk.");
    }
}

void HelloTriangleApplication::createSurface() {
    if (glfwCreateWindowSurface(instance, window.getWindow(), nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("I can't set up the window, this shit is broken in pieces.");
    }
}
