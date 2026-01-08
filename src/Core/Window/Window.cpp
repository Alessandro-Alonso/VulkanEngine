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
        "NETAEngine - Fuck Engineering",
        "NETAEngine - Calculus Is Killing Me",
        "NETAEngine - Be An Engineer They Said, Fuck Them.",
        "NETAEngine - Fucking Kill Me",
        "NETAEngine - I Have A Love And Hate Relationship With Physics",
        "NETAEngine - 566976612065737061c3b161",
        "NETAEngine - Jeg elsker brunost",
        "NETAEngine - Make Ubisoft great again",
        "NETAEngine - Fuerza Valencia",
        "NETAEngine - The numbers. Mason. What do they mean?",
        "NETAEngine - Oh shit, here we go again.",
        "NETAEngine - Another day, the same shit.."
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