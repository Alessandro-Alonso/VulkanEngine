cmake_minimum_required(VERSION 3.20)

project(VulkanEngine VERSION 0.1.0 LANGUAGES C CXX)

# CPM para las dependencias
include(cmake/CPM.cmake)

# Vulkan y glslc
find_package(Vulkan REQUIRED COMPONENTS glslc)

# Dependencias de CPM
CPMAddPackage(
    NAME glfw
    GITHUB_REPOSITORY glfw/glfw
    GIT_TAG 3.4
    OPTIONS
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_DOCS OFF"
)

CPMAddPackage(
    NAME glm
    GITHUB_REPOSITORY g-truc/glm
    GIT_TAG 1.0.1
)

CPMAddPackage(
    NAME imgui
    GITHUB_REPOSITORY ocornut/imgui
    GIT_TAG docking
)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
)

target_include_directories(imgui PUBLIC
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/backends"
)

target_link_libraries(imgui PUBLIC Vulkan::Vulkan glfw)

# VMA
CPMAddPackage(
    NAME VulkanMemoryAllocator
    GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    GIT_TAG master
)

# Archivos Source
set(ENGINE_SOURCES
    src/main.cpp

    src/App/HelloTriangle/HelloTriangleApplication.cpp

    src/Assets/Mesh.cpp

    src/Renderer/Renderer.cpp
    src/Renderer/RenderContext.cpp

    src/Core/Window/Window.cpp

    src/Vulkan/Device/PhysicalDevice.cpp

    src/Vulkan/Images/VulkanImage.cpp
    src/Vulkan/Images/VulkanImageView.cpp

    src/Vulkan/Pipeline/GraphicsPipeLine.cpp
    src/Vulkan/Pipeline/PipelineLayout.cpp

    src/Vulkan/Platform/FileSystem.cpp

    src/Vulkan/Swapchain/VulkanSwapChain.cpp

    src/Vulkan/Utils/ErrorHandling.cpp
    src/Vulkan/Utils/VmaImplementation.cpp
    src/Vulkan/Utils/VulkanRAII.cpp
    src/Vulkan/Utils/VulkanUtils.cpp

    src/Vulkan/Shaders/ShaderModule.cpp
)

# Creamos el ejecutable
add_executable(VulkanEngine ${ENGINE_SOURCES})

target_compile_features(VulkanEngine PRIVATE cxx_std_20)
target_include_directories(VulkanEngine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Conectamos dependencias
target_link_libraries(VulkanEngine PRIVATE
    Vulkan::Vulkan
    glfw
    glm::glm
    VulkanMemoryAllocator
)

if(MSVC)
    target_compile_options(VulkanEngine PRIVATE /W4 /wd4201)
    target_compile_definitions(VulkanEngine PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(VulkanEngine PRIVATE -Wall -Wextra)
endif()

if(WIN32)
    target_compile_definitions(VulkanEngine PRIVATE NOMINMAX)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug AND NOT MSVC)
    target_compile_options(VulkanEngine PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(VulkanEngine PRIVATE -fsanitize=address)
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "src" FILES ${ENGINE_SOURCES})

set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Vulkan/Shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/Shaders")

file(MAKE_DIRECTORY "${SHADER_BINARY_DIR}")

# Creamos un target custom para todos los shaders
add_custom_target(CompileShaders ALL)

function(compile_shader SOURCE_FILE SPV_FILE)
    set(SOURCE_PATH "${SHADER_SOURCE_DIR}/${SOURCE_FILE}")
    set(OUTPUT_PATH "${SHADER_BINARY_DIR}/${SPV_FILE}")

    add_custom_command(
        TARGET VulkanEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SHADER_BINARY_DIR}"
            "$<TARGET_FILE_DIR:VulkanEngine>/Shaders"
        COMMENT "Copying compiled shaders to output directory"
        VERBATIM
    )
endfunction()

compile_shader("shader.vert" "vert.spv")
compile_shader("shader.frag" "frag.spv")

add_dependencies(VulkanEngine CompileShaders)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)