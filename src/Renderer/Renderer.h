#pragma once

#include "RenderContext.h"
#include "Vulkan/Pipeline/PipelineLayout.h"
#include "Vulkan/Pipeline/GraphicsPipeLine.h"

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
        void initVulkan();
        void createPipeline();
        void createCommandSystem();
        void createSyncObjects();

        void recordCommandBuffer(VkCommandBuffer cmd, uint32_t imageIndex);

        RenderContext context;
        Window& window;

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