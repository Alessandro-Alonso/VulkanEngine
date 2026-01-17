#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "HelloTriangleApplication.h"
#include "Vulkan/Utils/ErrorHandling.h"
#include "Vulkan/Utils/VulkanUtils.h"

#ifdef __APPLE__
#include "Platform/MacOS/VulkanPlatform.h"
#endif


namespace NETAEngine {

    HelloTriangleApplication::HelloTriangleApplication() = default;

    // El destructor de HelloTriangleApplication se encarga de limpiar los recursos de Vulkan,
    // como el dispositivo logico, la superficie, el debug messenger y la instancia, ademas
    // de limpiar la ventana glfw.
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

    // Esto es el run, donde se inicializa la ventana, se inicializa Vulkan y se entra en el main loop.
    void HelloTriangleApplication::run() {
        m_window.initWindow();
        initVulkan();
        mainLoop();
    }

    // Esto se encarga de inicializar Vulkan, creando la instancia, el debug messenger, la superficie y el renderer.
    void HelloTriangleApplication::initVulkan() {
        
        createInstance();
        setupDebugMessenger();
        createSurface();

        m_renderer = std::make_unique<Renderer>(m_window, m_instance, m_surface);
    }

    // El mainloop es el bucle principal de la aplicacion, donde se procesan los eventos de la ventana y se renderiza cada frame,
    // siempre y cuando la ventana no este minimizada, ya que si esta minimizada, el tamaño del framebuffer es 0 (como ese video que te enseñe).
    void HelloTriangleApplication::mainLoop() {
        while (!glfwWindowShouldClose(m_window.getWindow())) {
            glfwPollEvents();

            int width, height;
            glfwGetFramebufferSize(m_window.getWindow(), &width, &height);

            // Only render if the window is visible (not minimized).
            if (width > 0 && height > 0 && m_renderer) {
                m_renderer->drawFrame();
            }
        }

        if (m_renderer) {
            m_renderer->waitIdle();
        }
    }

    // createInstance se encarga de crear la instancia de Vulkan, que es el primer paso para usar Vulkan, y se le pasan las extensiones
    // necesarias para que Vulkan funcione correctamente, ademas de las capas de validacion si estan habilitadas, y la informacion de la aplicacion.
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

#ifdef __APPLE__
        auto platformExtensions = macOSVulkan::getRequiredInstanceExtensions();
        extensions.insert(extensions.end(), platformExtensions.begin(), platformExtensions.end());
        createInfo.flags |= macOSVulkan::getInstanceCreateFlags();
#endif

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

    // populateDebugMessengerCreateInfo se encarga de llenar la estructura de VkDebugUtilsMessengerCreateInfoEXT, que es la que se va a usar para crear el
    // debug messenger, con la informacion de las capas de validacion, los tipos de mensajes que se quieren recibir y la funcion de callback.
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

    // setupDebugMessenger se encarga de crear el debug messenger, que es el encargado de recibir los mensajes de las capas de validacion,
    // y se le pasa la estructura de VkDebugUtilsMessengerCreateInfoEXT que se lleno en la funcion anterior, para que se cree con esa informacion.
    void HelloTriangleApplication::setupDebugMessenger() {
        if (!m_enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populateDebugMessengerCreateInfo(createInfo);

        // Error Handling.
        VK_CHECK(Utils::CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger));
    }

    // createSurface se encarga de crear la superficie de Vulkan, que es el encargado de representar la ventana en Vulkan, y se le pasa la ventana de GLFW.
    void HelloTriangleApplication::createSurface() {

        // Error Handling.
        VK_CHECK(glfwCreateWindowSurface(m_instance, m_window.getWindow(), nullptr, &m_surface));
    }
}