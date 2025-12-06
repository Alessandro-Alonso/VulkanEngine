#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanSwapChain;
class RenderPass;
class RenderContext;

class FrameManager {
public:
    FrameManager(const RenderContext& context, VkRenderPass renderPass, const VulkanSwapChain& swapChain);
    ~FrameManager();

    const std::vector<VkFramebuffer>& getFramebuffers() const { return framebuffers; }

private:
    const RenderContext& context;
    std::vector<VkFramebuffer> framebuffers;
};