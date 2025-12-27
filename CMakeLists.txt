cmake_minimum_required(VERSION 3.20)

project(VulkanEngine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE ENGINE_SOURCES src/*.cpp)
file(GLOB_RECURSE IMGUI_SOURCES imgui/*.cpp)

add_executable(VulkanEngine ${ENGINE_SOURCES})
target_include_directories(VulkanEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC imgui imgui/backends)
target_link_libraries(imgui PUBLIC Vulkan::Vulkan glfw)
target_link_libraries(VulkanEngine PRIVATE imgui)

include(cmake/CPM.cmake)

find_package(Vulkan REQUIRED)
target_link_libraries(VulkanEngine PRIVATE Vulkan::Vulkan)

CPMAddPackage("gh:glfw/glfw#3.4")
target_link_libraries(VulkanEngine PRIVATE glfw)

CPMAddPackage("gh:g-truc/glm#1.0.1")
target_link_libraries(VulkanEngine PRIVATE glm::glm)

if(MSVC)
    target_compile_options(VulkanEngine PRIVATE /W4)
else()
    target_compile_options(VulkanEngine PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug-Mode" AND NOT MSVC)
    target_compile_options(VulkanEngine PRIVATE -fsanitize=address)
    target_link_options(VulkanEngine PRIVATE -fsanitize=address)
endif()

if(WIN32)
    target_compile_definitions(VulkanEngine PRIVATE NOMINMAX)
endif()

if(APPLE)
    target_link_libraries(VulkanEngine PRIVATE "-framework Cocoa" "-framework IOKit" "-framework QuartzCore" "-framework Metal" "-framework CoreVideo" "-framework IOSurface")
    target_compile_definitions(VulkanEngine PRIVATE VK_USE_PLATFORM_MACOS_MVK)
endif()

set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Vulkan/Shaders")
set(SHADER_OUTPUT_DIR "$<TARGET_FILE_DIR:VulkanEngine>/Vulkan/Shaders")

add_custom_command(TARGET VulkanEngine PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUTPUT_DIR})

function(compile_shader SHADER_FILENAME OUTPUT_FILENAME)
    set(SOURCE "${SHADER_SOURCE_DIR}/${SHADER_FILENAME}")
    set(OUTPUT "${SHADER_OUTPUT_DIR}/${OUTPUT_FILENAME}")
    add_custom_command(TARGET VulkanEngine POST_BUILD COMMAND ${Vulkan_GLSLC_EXECUTABLE} "${SOURCE}" -o "${OUTPUT}" DEPENDS "${SOURCE}" VERBATIM)
endfunction()

compile_shader("shader.vert" "vert.spv")
compile_shader("shader.frag" "frag.spv")