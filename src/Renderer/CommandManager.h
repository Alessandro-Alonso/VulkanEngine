#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

class RenderContext;
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
    VkRenderPass renderPass;
    VkPipeline graphicsPipeline;

    std::vector<VkCommandBuffer> commandBuffers;

    struct CommandPoolDeleter {
        const RenderContext& context;
        void operator()(VkCommandPool* pool) const;
    };

    std::unique_ptr<VkCommandPool, CommandPoolDeleter> commandPool;

    void createCommandPool();
};
