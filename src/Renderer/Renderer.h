#pragma once

#include "RenderContext.h"
#include "Vulkan/Pipeline/PipelineLayout.h"
#include "Vulkan/Pipeline/GraphicsPipeLine.h"
#include "Vulkan\Utils\VulkanTypes.h"

#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

namespace NETAEngine {

    struct AllocatedImage {
        VkImage image = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
        VkExtent3D extent{};
        VkFormat format = VK_FORMAT_UNDEFINED;
    };

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

        void createDrawImage();
        void destroyDrawImage();

        void recordCommandBuffer(VkCommandBuffer cmd, uint32_t imageIndex);

        Window& window;
        RenderContext context;

        AllocatedImage m_drawImage;
        const VkFormat m_drawFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

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