#pragma once

#include <GLFW/glfw3.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class Window {
public:
    void initWindow();
    void cleanup();
    GLFWwindow* getWindow() const { return window; }

private:
    GLFWwindow* window = nullptr;
};