cmake_minimum_required(VERSION 3.20)

project(VulkanEngine VERSION 0.1.0 LANGUAGES C CXX)

# Dependencias
include(cmake/CPM.cmake)

# Encuentra el sistema de Vulkan
find_package(Vulkan REQUIRED COMPONENTS glslc)

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

# Imgui
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

add_executable(VulkanEngine ${ENGINE_SOURCES})

# Ajustes de compilacion
target_compile_features(VulkanEngine PRIVATE cxx_std_20)

target_include_directories(VulkanEngine PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${glfw_SOURCE_DIR}/include"
)

if(WIN32)
    target_compile_definitions(VulkanEngine PRIVATE VK_USE_PLATFORM_WIN32_KHR NOMINMAX _CRT_SECURE_NO_WARNINGS)
elseif(APPLE)
    target_compile_definitions(VulkanEngine PRIVATE VK_USE_PLATFORM_MACOS_MVK)
elseif(UNIX)
    target_compile_definitions(VulkanEngine PRIVATE VK_USE_PLATFORM_XCB_KHR)
endif()

target_link_libraries(VulkanEngine PRIVATE
    Vulkan::Vulkan
    glfw
    glm::glm
    imgui
    VulkanMemoryAllocator
)

# Compilacion de shaders
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Vulkan/Shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/Shaders")
file(MAKE_DIRECTORY "${SHADER_BINARY_DIR}")

function(compile_shader SOURCE_FILE SPV_FILE)
    set(SOURCE_PATH "${SHADER_SOURCE_DIR}/${SOURCE_FILE}")
    set(OUTPUT_PATH "${SHADER_BINARY_DIR}/${SPV_FILE}")
   
    # Solo compila si el source is mas nuevo
    add_custom_command(
        OUTPUT ${OUTPUT_PATH}
        COMMAND Vulkan::glslc "${SOURCE_PATH}" -o "${OUTPUT_PATH}"
        DEPENDS "${SOURCE_PATH}"
        COMMENT "Compiling ${SOURCE_FILE} to ${SPV_FILE}"
    )
    add_custom_target(Shader_${SOURCE_FILE} DEPENDS ${OUTPUT_PATH})
    add_dependencies(VulkanEngine Shader_${SOURCE_FILE})
endfunction()

# Compilar shaders, solo si han sido cambiados.
compile_shader("shader.vert" "vert.spv")
compile_shader("shader.frag" "frag.spv")
compile_shader("fullscreen.vert" "fullscreen.spv")
compile_shader("tonemapping.frag" "tonemapping.spv")

# Copia los shaders compilados a la carpeta del ejecutable
add_custom_command(
    TARGET VulkanEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SHADER_BINARY_DIR}"
        "$<TARGET_FILE_DIR:VulkanEngine>/Shaders"
)