cmake_minimum_required(VERSION 3.29)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "" FORCE)
endif()

project(neta_engine VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Esto es un linker que uso para hacer el compile mas rapido.
if(UNIX AND NOT APPLE)
    find_program(MOLD_LINKER mold)
    if(MOLD_LINKER)
        set(CMAKE_LINKER_TYPE "mold")
        message(STATUS "Using mold linker")
    else()
        find_program(LLD_LINKER lld)
        if(LLD_LINKER)
            set(CMAKE_LINKER_TYPE "lld")
            message(STATUS "Using lld linker")
        endif()
    endif()
endif()

# Un cache para el compilador, de esta forma no compilamos archivos que sean o fueron cambiados.
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "Compiler cache enabled (ccache)")
else()
    find_program(SCCACHE_PROGRAM sccache)
    if(SCCACHE_PROGRAM)
        set(CMAKE_C_COMPILER_LAUNCHER "${SCCACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${SCCACHE_PROGRAM}")
        message(STATUS "Compiler cache enabled (sccache)")
    endif()
endif()

include(cmake/CPM.cmake)

find_package(Vulkan REQUIRED)

# GLFW
CPMAddPackage(
    NAME glfw
    GITHUB_REPOSITORY glfw/glfw
    GIT_TAG 3.4
    OPTIONS "GLFW_BUILD_EXAMPLES OFF" "GLFW_BUILD_TESTS OFF" "GLFW_BUILD_DOCS OFF"
)

# GLM
CPMAddPackage(
    NAME glm
    GITHUB_REPOSITORY g-truc/glm
    GIT_TAG 1.0.1
)

# ImGui
CPMAddPackage(
    NAME imgui
    GITHUB_REPOSITORY ocornut/imgui
    GIT_TAG docking
)

# Vulkan Memory Allocator (VMA)
CPMAddPackage(
    NAME VulkanMemoryAllocator
    GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    GIT_TAG master
)

# ImGui library
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
)

target_compile_definitions(imgui PUBLIC IMGUI_DEFINE_MATH_OPERATORS)

target_precompile_headers(imgui PRIVATE
    <vector>
    <string>
    "${imgui_SOURCE_DIR}/imgui.h"
)

target_include_directories(imgui PUBLIC
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/backends"
)
target_link_libraries(imgui PUBLIC Vulkan::Vulkan glfw)
target_compile_options(imgui PRIVATE -w)

# Engine sources
set(ENGINE_SOURCES
    src/main.cpp
    src/App/HelloTriangle/HelloTriangleApplication.cpp
    src/Assets/Mesh.cpp
    src/Renderer/Renderer.cpp
    src/Renderer/RenderContext.cpp
    src/Core/Window/Window.cpp
    src/Vulkan/Device/PhysicalDevice.cpp
    src/Vulkan/Pipeline/GraphicsPipeLine.cpp
    src/Vulkan/Pipeline/PipelineLayout.cpp
    src/Vulkan/Platform/FileSystem.cpp
    src/Vulkan/Swapchain/VulkanSwapChain.cpp
    src/Vulkan/Utils/ErrorHandling.cpp
    src/Vulkan/Utils/VmaImplementation.cpp
    src/Vulkan/Utils/VulkanRAII.cpp
    src/Vulkan/Utils/VulkanUtils.cpp
    src/Vulkan/Shaders/ShaderModule.cpp
    src/Vulkan/Images/AllocatedImage.cpp
)

add_executable(neta_engine ${ENGINE_SOURCES})

# Agrega Unity para compilar mas rapido
set_target_properties(neta_engine PROPERTIES
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 8
)

target_compile_features(neta_engine PRIVATE cxx_std_20)

target_include_directories(neta_engine PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${glfw_SOURCE_DIR}/include"
    "${VulkanMemoryAllocator_SOURCE_DIR}/include"
)

# Pre-compila todos estos headers que son muy comunes para que tarde menos en compilar.
target_precompile_headers(neta_engine PRIVATE
    <vector>
    <string>
    <memory>
    <stdexcept>
    <iostream>
    <vulkan/vulkan.h>
    <glm/glm.hpp>
)

# En Windows, definimos VK_USE_PLATFORM_WIN32_KHR para usar la extension de superficie de Win32.
if(WIN32)
    target_compile_definitions(neta_engine PRIVATE VK_USE_PLATFORM_WIN32_KHR NOMINMAX _CRT_SECURE_NO_WARNINGS)
elseif(APPLE)
    target_compile_definitions(neta_engine PRIVATE VK_USE_PLATFORM_METAL_EXT)
    target_compile_definitions(imgui PRIVATE VK_USE_PLATFORM_METAL_EXT)

    find_library(COCOA Cocoa REQUIRED)
    find_library(IOKIT IOKit REQUIRED)
    find_library(CORE_VIDEO CoreVideo REQUIRED)
    find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
    find_library(QUARTZ_CORE QuartzCore REQUIRED)
    find_library(METAL Metal REQUIRED)
    find_library(IO_SURFACE IOSurface REQUIRED)

    target_link_libraries(neta_engine PRIVATE
        ${COCOA}
        ${IOKIT}
        ${CORE_VIDEO}
        ${CORE_FOUNDATION}
        ${QUARTZ_CORE}
        ${METAL}
        ${IO_SURFACE}
    )

    if(DEFINED ENV{VULKAN_SDK})
        target_link_directories(neta_engine PRIVATE "$ENV{VULKAN_SDK}/lib")
    else()
        message(WARNING "VULKAN_SDK environment variable is not set!")
    endif()

elseif(UNIX)
    target_compile_definitions(neta_engine PRIVATE VK_USE_PLATFORM_XCB_KHR)
endif()

target_link_libraries(neta_engine PRIVATE
    Vulkan::Vulkan
    glfw
    glm::glm
    imgui
    VulkanMemoryAllocator
)

# Definir directorios para los shaders fuente y compilados
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Vulkan/Shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/Shaders")
file(MAKE_DIRECTORY "${SHADER_BINARY_DIR}")

function(compile_shader SOURCE_FILE SPV_FILE)
    set(SOURCE_PATH "${SHADER_SOURCE_DIR}/${SOURCE_FILE}")
    set(OUTPUT_PATH "${SHADER_BINARY_DIR}/${SPV_FILE}")

    add_custom_command(
        OUTPUT ${OUTPUT_PATH}
        COMMAND Vulkan::glslc "${SOURCE_PATH}" -o "${OUTPUT_PATH}"
        DEPENDS "${SOURCE_PATH}"
        COMMENT "Compiling ${SOURCE_FILE} to ${SPV_FILE}"
    )
    add_custom_target(Shader_${SOURCE_FILE} DEPENDS ${OUTPUT_PATH})
    add_dependencies(neta_engine Shader_${SOURCE_FILE})
endfunction()

compile_shader("shader.vert" "vert.spv")
compile_shader("shader.frag" "frag.spv")
compile_shader("fullscreen.vert" "fullscreen.spv")
compile_shader("tonemapping.frag" "tonemapping.spv")

# Copiar los shaders compilados al directorio de salida del ejecutable
add_custom_command(
    TARGET neta_engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SHADER_BINARY_DIR}"
        "$<TARGET_FILE_DIR:neta_engine>/Shaders"
)