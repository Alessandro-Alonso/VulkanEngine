#include "Window.h"
#include <stdexcept>
#include <vector>
#include <random>

void Window::framebufferResizeCallback(GLFWwindow* window, int width, int heigth) {
    auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

// Textos randoms para la ventana
const char* Window::generateRandomTitle() {

    static const std::vector<const char*> titles = {
        "NETAEngine - Fuck Engineering.    (VULKAN v1.4.341.0)",
        "NETAEngine - Calculus Is Killing Me.   (VULKAN v1.4.341.0)",
        "NETAEngine - Be An Engineer They Said, Fuck Them.  (VULKAN v1.4.341.0)",
        "NETAEngine - Fucking Kill Me.  (VULKAN v1.4.341.0)",
        "NETAEngine - I Have A Love And Hate Relationship With Physics. (VULKAN v1.4.341.0)",
        "NETAEngine - 566976612065737061c3b161     (VULKAN v1.4.341.0)",
        "NETAEngine - Jeg elsker brunost   (VULKAN v1.4.341.0)",
        "NETAEngine - Fuerza Valencia   (VULKAN v1.4.341.0)",
        "NETAEngine - The numbers. Mason. What do they mean?   (VULKAN v1.4.341.0)",
        "NETAEngine - Oh shit, here we go again.    (VULKAN v1.4.341.0)",
        "NETAEngine - Another day, the same shit..  (VULKAN v1.4.341.0)",
        "NETAEngine - I'm not a real engineer, I'm just a guy who knows how to use a computer.  (VULKAN v1.4.341.0)",
        "NETAEngine - I have a love-hate relationship with engineering this garbage.    (VULKAN v1.4.341.0)",
        "NETAEngine - Oh, a window with this title!   (VULKAN v1.4.341.0)",
        "NETAEngine - This is the best engine ever created. (VULKAN v1.4.341.0)"
    };

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> distr(0, titles.size() - 1);

    return titles[distr(gen)];
}

// Ventana de GLFW, donde se renderiza la aplicacion.
void Window::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    #ifdef __APPLE__
        // Esto sinceramente es opcional, pero lo agrego para que funcione bien con macOS.
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
    #endif
    
    const char* windowTitle = generateRandomTitle();

    window = glfwCreateWindow(WIDTH, HEIGHT, windowTitle, nullptr, nullptr);
    if (!window) throw std::runtime_error("Failed to create GLFW window!");

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

}

void Window::cleanup() {
   if (window) {
    glfwDestroyWindow(window);
    window= nullptr;
   }

   glfwTerminate(); // Borra la ventana de la memoria.
}