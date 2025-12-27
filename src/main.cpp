#include "App/HelloTriangle/HelloTriangleApplication.h"
#include <iostream>

int main() {
    NETAEngine::HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}