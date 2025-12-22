cmake_minimum_required(VERSION 3.20)

project(VulkanEngine VERSION 0.1.0 LANGUAGES C CXX)

# Hacemos el CMAKE standar a 20.
set(CMAKE_CXX_STANDARD 20)

# Ejecutable del motor y la carpeta del codigo entero/src.
file(GLOB_RECURSE ENGINE_SOURCES src/*.cpp)
file(GLOB_RECURSE IMGUI_SOURCES imgui/*.cpp)

add_executable(VulkanEngine ${ENGINE_SOURCES})
target_include_directories(VulkanEngine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Librerias de Imgui y directorios.
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC imgui imgui/backends)
target_link_libraries(imgui PUBLIC Vulkan::Vulkan glfw)
target_link_libraries(VulkanEngine PRIVATE imgui)

# CPM package manager
include(cmake/CPM.cmake)

# Busca los paquetes de Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(VulkanEngine PRIVATE Vulkan::Vulkan)

# Agrega los paquetes de GLFW
CPMAddPackage("gh:glfw/glfw#3.4")
target_link_libraries(VulkanEngine PRIVATE glfw)

# Agrega los paquetes de GLM
CPMAddPackage("gh:g-truc/glm#1.0.1")
target_link_libraries(VulkanEngine PRIVATE glm::glm)

# Esto es para activar los warnings.
if(MSVC)
    target_compile_options(VulkanEngine PRIVATE /W4)
else()
    target_compile_options(VulkanEngine PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC)
    target_compile_options(VulkanEngine PRIVATE -fsanitize=address)
    target_link_options(VulkanEngine PRIVATE -fsanitize=address)
endif()

if(WIN32)
    target_compile_definitions(VulkanEngine PRIVATE NOMINMAX)
endif()

# macOS frameworks (necesario para GLFW + Vulkan/MoltenVK)
if(APPLE)
    target_link_libraries(VulkanEngine PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework QuartzCore"
        "-framework Metal"
        "-framework CoreVideo"
        "-framework IOSurface"
    )
    target_compile_definitions(VulkanEngine PRIVATE VK_USE_PLATFORM_MACOS_MVK)
endif()

# Copiar los shaders a la carpeta build.
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Vulkan/Shaders)

# Copia los shaders al la ruta del ejecutable en cada build.
file(GLOB SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*")

add_custom_command(
    TARGET VulkanEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${SHADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/Vulkan/Shaders/
)