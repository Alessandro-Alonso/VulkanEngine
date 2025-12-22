#pragma once

#include <vulkan/vulkan.h>

class RenderPass {
public:
    RenderPass(VkDevice device, VkFormat swapchainImageFormat);
    ~RenderPass();

    VkRenderPass get() const { return renderPass; }

private:
    VkDevice device;
    VkRenderPass renderPass{ VK_NULL_HANDLE };
};