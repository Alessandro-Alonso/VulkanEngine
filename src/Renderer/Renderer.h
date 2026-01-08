#pragma once

#include "RenderContext.h"
#include "Vulkan/Pipeline/PipelineLayout.h"
#include "Vulkan/Pipeline/GraphicsPipeLine.h"
#include "Vulkan/Utils/VulkanTypes.h"
#include "Vulkan/Images/AllocatedImage.h"

#include <vector>
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
        static constexpr const char* SHADER_DIR = "Shaders";

        void initVulkan();
        void createPipeline();
        void createCommandSystem();
        void createSyncObjects();
        void createImages();


        void initDescriptors();
        void updateDescriptorSets();

        VkSampler m_defaultSampler = VK_NULL_HANDLE;

        VkDescriptorSetLayout m_postProcessDescriptorLayout = VK_NULL_HANDLE;
        VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
        VkDescriptorSet m_postProcessDescriptorSet = VK_NULL_HANDLE;

        void recordCommandBuffer(VkCommandBuffer cmd, uint32_t imageIndex);
        void transitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);

        Window& window;
        RenderContext context;

        std::unique_ptr<PipelineLayout> m_postProcessLayout;
        std::unique_ptr<GraphicsPipeline> m_postProcessPipeline;
        std::unique_ptr<AllocatedImage> m_drawImage;
        std::unique_ptr<AllocatedImage> m_depthImage;

        const VkFormat m_drawFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        const VkFormat m_depthFormat = VK_FORMAT_D32_SFLOAT;

        // Recursos del pipeline (no es el renderpass)
        std::unique_ptr<PipelineLayout> pipelineLayout;
        std::unique_ptr<GraphicsPipeline> graphicsPipeline;

        // Recursos de commando
        VkCommandPool commandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> commandBuffers;

        // Syncronizacion
        const int MAX_FRAMES_IN_FLIGHT = 2;
        uint32_t currentFrame = 0;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
    };
}