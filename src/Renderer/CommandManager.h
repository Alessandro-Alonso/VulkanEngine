#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class RenderContext;
class RenderPass;
class GraphicsPipeline;
class VulkanSwapChain;

class CommandManager {
public:
    CommandManager(
        const RenderContext& context,
        VkRenderPass renderPass,
        VkPipeline graphicsPipeline, 
        const VulkanSwapChain& swapChain
    );
    ~CommandManager();

    void recordCommands(const std::vector<VkFramebuffer>& framebuffers, const VulkanSwapChain& swapChain);

    VkCommandBuffer getCommandBuffer(uint32_t index) const { return commandBuffers[index]; }
    uint32_t getCommandBufferCount() const { return static_cast<uint32_t>(commandBuffers.size()); }

private:
    const RenderContext& context;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers;

    VkRenderPass renderPass;
    VkPipeline graphicsPipeline;

    void createCommandPool();
};