#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <optional>
#include <set>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <cstdint>
#include "Vulkan/Utils/VulkanTypes.h"

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanSwapChain {
public:
    VulkanSwapChain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, GLFWwindow* window, const QueueFamilyIndices& indices);
    ~VulkanSwapChain();

    void cleanup();

    VkFormat getImageFormat() const { return swapChainImageFormat; }
    VkExtent2D getExtent() const { return swapChainExtent; }
    VkSwapchainKHR getSwapChain() const { return swapChain; }
    const std::vector<VkImage>& getImages() const { return swapChainImages; }
    const std::vector<VkImageView>& getImageViews() const { return swapChainImageViews; }
    std::vector<VkImageView> swapChainImageViews;


private:
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;
    GLFWwindow* window;
    QueueFamilyIndices queueIndices;

    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

private:
    void createSwapChain();

    // Ayudas
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void createImageViews();

};