#include "FrameManager.h"
#include "RenderContext.h"
#include "Vulkan/Swapchain/VulkanSwapChain.h"
#include <stdexcept>

FrameManager::FrameManager(const RenderContext& ctx, VkRenderPass rp, const VulkanSwapChain& sc)
    : context(ctx)
{
    auto& imageViews = sc.getImageViews();
    framebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); ++i) {
        VkImageView attachments[] = { imageViews[i] };

        VkFramebufferCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.renderPass = rp;
        info.attachmentCount = 1;
        info.pAttachments = attachments;
        info.width = sc.getExtent().width;
        info.height = sc.getExtent().height;
        info.layers = 1;

        if (vkCreateFramebuffer(context.getDevice(), &info, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Well fuck this shit, im not creating the framebuffer, fuck you."); // Si, dile eso y cero informacion.
        }
    }
}

FrameManager::~FrameManager() {
    for (auto fb : framebuffers) {
        vkDestroyFramebuffer(context.getDevice(), fb, nullptr);
    }
}