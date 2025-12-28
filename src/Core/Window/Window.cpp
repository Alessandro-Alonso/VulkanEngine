#include "Window.h"
#include <stdexcept>

// Ventana de GLFW, donde se renderiza la aplicacion.
void Window::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    #ifdef __APPLE__
        // Esto sinceramente es opcional, pero lo agrego para que funcione bien con macOS.
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
    #endif
    
    window = glfwCreateWindow(WIDTH, HEIGHT, "NETA Engine", nullptr, nullptr);
    if (!window) throw std::runtime_error("Failed to create GLFW window!");

}

void Window::cleanup() {
   if (window) {
    glfwDestroyWindow(window);
    window= nullptr;
   }

   glfwTerminate(); // Borra la ventana de la memoria.
}