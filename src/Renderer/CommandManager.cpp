#include "CommandManager.h"
#include "RenderContext.h"
#include "Vulkan/Swapchain/VulkanSwapChain.h"
#include "Vulkan/Utils/ErrorHandling.h"
#include <stdexcept>
#include <iostream>

void CommandManager::CommandPoolDeleter::operator()(VkCommandPool* pool) const {
    if (pool && *pool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(context.getDevice(), *pool, nullptr);
        delete pool;
    }
}

CommandManager::CommandManager(
    const RenderContext& ctx,
    VkRenderPass rp,
    VkPipeline pipeline,
    const VulkanSwapChain& sc)
    : context(ctx),
      renderPass(rp),
      graphicsPipeline(pipeline),
      commandPool(nullptr, CommandPoolDeleter{ctx})
{
    createCommandPool();
}

CommandManager::~CommandManager() {
    if (commandPool && !commandBuffers.empty()) {
        VkCommandPool pool = *commandPool;
        if (pool != VK_NULL_HANDLE) {
            vkFreeCommandBuffers(context.getDevice(), pool,
                                 static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        }
    }
}

void CommandManager::createCommandPool() {
    auto indices = context.getQueueFamilies();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = indices.graphicsFamily.value();

    VkCommandPool* poolPtr = new VkCommandPool{ VK_NULL_HANDLE };

    // Error Handling.
    VK_CHECK(vkCreateCommandPool(context.getDevice(), &poolInfo, nullptr, poolPtr));

    commandPool.reset(poolPtr);
}

void CommandManager::recordCommands(const std::vector<VkFramebuffer>& framebuffers, const VulkanSwapChain& swapChain) {
    if (framebuffers.empty()) {
        throw std::runtime_error("recordCommands: no framebuffers provided.");
    }

    if (!commandPool) {
        throw std::runtime_error("recordCommands: commandPool is null.");
    }

    VkCommandPool pool = *commandPool;
    if (pool == VK_NULL_HANDLE) {
        throw std::runtime_error("recordCommands: commandPool handle is VK_NULL_HANDLE.");
    }

    commandBuffers.resize(framebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    VkResult res = vkAllocateCommandBuffers(context.getDevice(), &allocInfo, commandBuffers.data());
    if (res != VK_SUCCESS) {
        commandBuffers.clear();
        throw std::runtime_error("recordCommands: vkAllocateCommandBuffers failed.");
    }

    for (size_t i = 0; i < commandBuffers.size(); ++i) {
        if (commandBuffers[i] == VK_NULL_HANDLE) {
            vkFreeCommandBuffers(context.getDevice(), pool,
                                 static_cast<uint32_t>(i), commandBuffers.data());
            commandBuffers.clear();
            throw std::runtime_error("recordCommands: vkAllocateCommandBuffers returned VK_NULL_HANDLE for an entry.");
        }
    }

    for (size_t i = 0; i < commandBuffers.size(); ++i) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VkResult bres = vkBeginCommandBuffer(commandBuffers[i], &beginInfo);
        if (bres != VK_SUCCESS) {
            throw std::runtime_error("recordCommands: vkBeginCommandBuffer failed.");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = framebuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain.getExtent();

        VkClearValue clearColor{};
        clearColor.color = { {0.0f, 0.0f, 0.2f, 1.0f} };
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

        VkResult endRes = vkEndCommandBuffer(commandBuffers[i]);
        if (endRes != VK_SUCCESS) {
            throw std::runtime_error("recordCommands: vkEndCommandBuffer failed.");
        }
    }
}
