cmake_minimum_required(VERSION 3.31.6)

project(VulkanEngine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

# Archivos fuente
add_executable(VulkanEngine
    src/main.cpp

    # RENDERER / RENDER CONTEXT
    src/Renderer/RenderContext.cpp
    src/Renderer/FrameManager.cpp
    src/Renderer/CommandManager.cpp
    src/Renderer/Renderer.cpp

    # APP / HelloTriangle
    src/App/HelloTriangle/HelloTriangleApplication.cpp

    # CORE / WINDOW
    src/Core/Window/Window.cpp

    # VULKAN / DEVICE
    src/Vulkan/Device/PhysicalDevice.cpp

    # VULKAN / IMAGES
    src/Vulkan/Images/VulkanImageView.cpp
    src/Vulkan/Images/VulkanImage.cpp

    # VULKAN / PIPELINE
    src/Vulkan/Pipeline/GraphicsPipeLine.cpp
    src/Vulkan/Pipeline/PipelineLayout.cpp
    src/Vulkan/Pipeline/RenderPass.cpp

    # VULKAN / PLATFORM
    src/Vulkan/Platform/FileSystem.cpp
    
    # VULKAN / SHADERS
    src/Vulkan/Shaders/ShaderModule.cpp

    # VULAKN / SWAPCHAIN
    src/Vulkan/Swapchain/VulkanSwapChain.cpp

    # VULKAN / UTILS
    src/Vulkan/Utils/VulkanUtils.cpp

    # ImGui los cpp de imgui, para que funcione al compilar.
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_vulkan.cpp
    imgui/backends/imgui_impl_glfw.cpp
)

# Incluir las carpetas de imgui, para que funcione.
target_include_directories(VulkanEngine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

# CPM package manager
include(cmake/CPM.cmake)

# Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(VulkanEngine PRIVATE Vulkan::Vulkan)

# GLFW
CPMAddPackage("gh:glfw/glfw#3.4")
target_link_libraries(VulkanEngine PRIVATE glfw)

# GLM
CPMAddPackage("gh:g-truc/glm#1.0.1")
target_link_libraries(VulkanEngine PRIVATE glm)

# macOS frameworks (necesario para GLFW + Vulkan/MoltenVK)
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(VulkanEngine PRIVATE ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
    target_compile_definitions(VulkanEngine PRIVATE VK_USE_PLATFORM_MACOS_MVK)
    target_link_libraries(VulkanEngine PRIVATE
        "-framework Cocoa"
        "-framework QuartzCore"
        "-framework Metal"
        "-framework IOSurface"
    )
endif()

# Copiar los shaders a la carpeta build.
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Vulkan/Shaders)

# Copia los shaders al la ruta del ejecutable en cada build.
file(GLOB SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/*")

add_custom_command(
    TARGET VulkanEngine PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${SHADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/Vulkan/Shaders/
)