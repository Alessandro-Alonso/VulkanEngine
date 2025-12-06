#include "CommandManager.h"
#include "RenderContext.h"
#include "Vulkan/Pipeline/GraphicsPipeLine.h"
#include "Vulkan/Swapchain/VulkanSwapChain.h"
#include <stdexcept>

CommandManager::CommandManager(
    const RenderContext& ctx,
    VkRenderPass rp,
    VkPipeline pipeline,
    const VulkanSwapChain& sc)
    : context(ctx), renderPass(rp), graphicsPipeline(pipeline)
{
    createCommandPool();
}

CommandManager::~CommandManager() {
    if (!commandBuffers.empty()) {
        vkFreeCommandBuffers(context.getDevice(), commandPool,
                             static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    }
    if (commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(context.getDevice(), commandPool, nullptr);
    }
}

void CommandManager::createCommandPool() {
    auto indices = context.getQueueFamilies();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = indices.graphicsFamily.value();

    if (vkCreateCommandPool(context.getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Well, i can't create command pool. I don't fucking know why. Time to do some research, good luck.");
    }
}

void CommandManager::recordCommands(const std::vector<VkFramebuffer>& framebuffers, const VulkanSwapChain& swapChain) {
    commandBuffers.resize(framebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(context.getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("I can't fucking allocate the command buffers, time to search it up.");
    }

    for (size_t i = 0; i < commandBuffers.size(); ++i) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(commandBuffers[i], &beginInfo);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;  // Usamos el que pasamos
        renderPassInfo.framebuffer = framebuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain.getExtent();

        VkClearValue clearColor = {0.0f, 0.0f, 0.2f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain.getExtent().width);
        viewport.height = static_cast<float>(swapChain.getExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffers[i], 0, 1, &viewport);

        VkRect2D scissor{{0, 0}, swapChain.getExtent()};
        vkCmdSetScissor(commandBuffers[i], 0, 1, &scissor);

        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
        vkCmdEndRenderPass(commandBuffers[i]);
        vkEndCommandBuffer(commandBuffers[i]);
    }
}