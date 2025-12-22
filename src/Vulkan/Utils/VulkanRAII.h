#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <functional>
#include <vector>
#include "ErrorHandling.h"

// RAII wrapper para los handles de vulkan.
template <typename HandleType, typename DeleterType>
using VulkanHandle = std::unique_ptr<HandleType, DeleterType>;

// La Instancia
inline void DestroyInstance(VkInstance instance, const VkAllocationCallbacks* allocator) {
    vkDestroyInstance(instance, allocator);
}
using InstanceHandle = VulkanHandle<VkInstance, decltype(&DestroyInstance)>;

// La Superficie
inline void DestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks* allocator) {
    vkDestroySurfaceKHR(instance, surface, allocator);
}
using SurfaceHandle = VulkanHandle<VkSurfaceKHR, std::function<void(VkSurfaceKHR)>>;

// Los mensajes de debug
inline void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* allocator) {
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (func) func(instance, messenger, allocator);
}
using DebugMessengerHandle = VulkanHandle<VkDebugUtilsMessengerEXT, std::function<void(VkDebugUtilsMessengerEXT)>>;

// Los Dispositivos
inline void DestroyDevice(VkDevice device, const VkAllocationCallbacks* allocator) {
    vkDestroyDevice(device, allocator);
}
using DeviceHandle = VulkanHandle<VkDevice, decltype(&DestroyDevice)>;

// El Swapchain
inline void DestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks* allocator) {
    vkDestroySwapchainKHR(device, swapchain, allocator);
}
using SwapchainHandle = VulkanHandle<VkSwapchainKHR, std::function<void(VkSwapchainKHR)>>;

// El ImageView
inline void DestroyImageView(VkDevice device, VkImageView view, const VkAllocationCallbacks* allocator) {
    vkDestroyImageView(device, view, allocator);
}
using ImageViewHandle = VulkanHandle<VkImageView, std::function<void(VkImageView)>>;

// Los ShaderModule
inline void DestroyShaderModule(VkDevice device, VkShaderModule module, const VkAllocationCallbacks* allocator) {
    vkDestroyShaderModule(device, module, allocator);
}
using ShaderModuleHandle = VulkanHandle<VkShaderModule, std::function<void(VkShaderModule)>>;

// ShaderPipeline
inline void DestroyPipelineLayout(VkDevice device, VkPipelineLayout layout, const VkAllocationCallbacks* allocator) {
    vkDestroyPipelineLayout(device, layout, allocator);
}
using PipelineLayoutHandle = VulkanHandle<VkPipelineLayout, std::function<void(VkPipelineLayout)>>;

// El RenderPass
inline void DestroyRenderPass(VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks* allocator) {
    vkDestroyRenderPass(device, renderPass, allocator);
}
using RenderPassHandle = VulkanHandle<VkRenderPass, std::function<void(VkRenderPass)>>;

// El Pipeline
inline void DestroyPipeline(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* allocator) {
    vkDestroyPipeline(device, pipeline, allocator);
}
using PipelineHandle = VulkanHandle<VkPipeline, std::function<void(VkPipeline)>>;

// El Framebuffer
inline void DestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* allocator) {
    vkDestroyFramebuffer(device, framebuffer, allocator);
}
using FramebufferHandle = VulkanHandle<VkFramebuffer, std::function<void(VkFramebuffer)>>;

// Los CommandPool
inline void DestroyCommandPool(VkDevice device, VkCommandPool pool, const VkAllocationCallbacks* allocator) {
    vkDestroyCommandPool(device, pool, allocator);
}
using CommandPoolHandle = VulkanHandle<VkCommandPool, std::function<void(VkCommandPool)>>;

// Esto lo pongo para hacer los identificadores de forma segura. Es algo opcional sinceramente, pero lo pongo para hacerlo mas "profesional". Es esto profesional?
template <typename HandleType, typename CreateFunc, typename... Args>
HandleType CreateVulkanHandle(CreateFunc createFunc, Args&&... args) {
    HandleType handle = VK_NULL_HANDLE;
    VkResult result = createFunc(std::forward<Args>(args)..., &handle);
    VK_CHECK(result);
    return handle;
}