#pragma once

#include "RenderContext.h"
#include "Vulkan/Pipeline/PipelineLayout.h"
#include "Vulkan/Pipeline/GraphicsPipeLine.h"
#include "Vulkan/Images/AllocatedImage.h"
#include "vulkan/vulkan_core.h"

#include <array>
#include <memory>
#include <vulkan/vulkan.h>

namespace NETAEngine {

    class Renderer {
    public:
        Renderer(Window& window, VkInstance instance, VkSurfaceKHR surface);
        ~Renderer();

        // Quitamos copia
        Renderer(const Renderer&) = delete;
        Renderer& operator = (const Renderer&) = delete;

        void drawFrame();
        void waitIdle() const { context.waitIdle(); }

        void recreateSwapChain();

    private:
        RenderContext context;
        Window& window;

        std::unique_ptr<AllocatedImage> m_drawImage;
        std::unique_ptr<AllocatedImage> m_depthImage;
        std::unique_ptr<PipelineLayout> m_postProcessLayout;
        std::unique_ptr<GraphicsPipeline> m_postProcessPipeline;

        // Recursos del pipeline (no es el renderpass)
        std::unique_ptr<PipelineLayout> pipelineLayout;
        std::unique_ptr<GraphicsPipeline> graphicsPipeline;

        static constexpr uint32_t DESCRIPTOR_POOL_MAX_SETS = 100;
        static constexpr uint32_t DESCRIPTOR_POOL_SAMPLER_COUNT = 100;

        static constexpr const char* SHADER_DIR = "Shaders";

        void initVulkan();
        void initFrameData();
        void createPipeline();
        void createImages();
        void initDescriptors();
        void updateDescriptorSets();

        VkSampler m_defaultSampler = VK_NULL_HANDLE;

        VkDescriptorSetLayout m_postProcessDescriptorLayout = VK_NULL_HANDLE;
        VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
        VkDescriptorSet m_postProcessDescriptorSet = VK_NULL_HANDLE;

        void recordCommandBuffer(VkCommandBuffer cmd, uint32_t imageIndex);
        void transitionImage(VkCommandBuffer cmd, VkImage image,
            VkImageLayout currentLayout, VkImageLayout newLayout,
            VkPipelineStageFlags2 srcStage, VkAccessFlags2 srcAccess,
            VkPipelineStageFlags2 dstStage, VkAccessFlags2 dstAccess);


        const VkFormat m_drawFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        const VkFormat m_depthFormat = VK_FORMAT_D32_SFLOAT;

        struct FrameData {
            VkCommandPool commandPool           = VK_NULL_HANDLE;
            VkCommandBuffer commandBuffer       = VK_NULL_HANDLE;
            VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
            VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
            VkFence inFlightFence               = VK_NULL_HANDLE;
        };

        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        std::array<FrameData, MAX_FRAMES_IN_FLIGHT> m_frames;
        uint32_t m_currentFrame = 0;
        
        std::vector<VkSemaphore> m_renderFinishedSemaphores;

        VkCommandPool transferCommandPool = VK_NULL_HANDLE;

        FrameData& getCurrentFrame() { return m_frames[m_currentFrame]; }

    };
}