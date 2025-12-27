#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "HelloTriangleApplication.h"
#include "Vulkan/Utils/ErrorHandling.h"
#include "Vulkan/Utils/VulkanUtils.h"

namespace NETAEngine {

    HelloTriangleApplication::HelloTriangleApplication() = default;

    HelloTriangleApplication::~HelloTriangleApplication() {

        m_renderer.reset();

        if (m_enableValidationLayers && m_debugMessenger != VK_NULL_HANDLE) {
            Utils::DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
        }

        if (m_surface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        }

        if (m_instance != VK_NULL_HANDLE) {
            vkDestroyInstance(m_instance, nullptr);
        }

        m_window.cleanup();
    }

    void HelloTriangleApplication::run() {
        m_window.initWindow();
        initVulkan();
        mainLoop();
    }

    void HelloTriangleApplication::initVulkan() {
        createInstance();
        setupDebugMessenger();
        createSurface();

        m_renderer = std::make_unique<Renderer>(m_window, m_instance, m_surface);
    }

    void HelloTriangleApplication::mainLoop() {
        while (!glfwWindowShouldClose(m_window.getWindow())) {
            glfwPollEvents();
            if (m_renderer) {
                m_renderer->drawFrame();
            }
        }

        if (m_renderer) {
            m_renderer->waitIdle();
        }
    }

    void HelloTriangleApplication::createInstance() {
        if (m_enableValidationLayers && !Utils::checkValidationLayerSupport(m_validationLayers)) {
            GE_CHECK(false, "Requested validation layers are not available");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "NETA Engine";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "NETA Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = Utils::getRequiredExtensions(m_enableValidationLayers);
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

        // Validation Layers
        if (m_enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
            createInfo.ppEnabledLayerNames = m_validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = &debugCreateInfo;
        }
        else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        // Error Handling.
        VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_instance));

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

        createInfo.pfnUserCallback = Utils::debugCallback;
        createInfo.pUserData = nullptr;
    }

    void HelloTriangleApplication::setupDebugMessenger() {
        if (!m_enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populateDebugMessengerCreateInfo(createInfo);

        // Error Handling.
        VK_CHECK(Utils::CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger));
    }

    void HelloTriangleApplication::createSurface() {

        // Error Handling.
        VK_CHECK(glfwCreateWindowSurface(m_instance, m_window.getWindow(), nullptr, &m_surface));
    }
}