#pragma once

#include <cstdint>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class Window {
public:
    void initWindow();
    void cleanup();
    GLFWwindow* getWindow() const { return window; }

    bool wasResized() { return framebufferResized; }
    void resetWindowResizedFlag() { framebufferResized = false; }

private:
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    const char* generateRandomTitle();

    GLFWwindow* window = nullptr;
    bool framebufferResized = false;
};