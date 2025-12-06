#pragma once

#include "RenderContext.h"
#include "CommandManager.h"
#include "FrameManager.h"
#include "Vulkan/Pipeline/RenderPass.h"
#include "Vulkan/Pipeline/PipelineLayout.h"
#include "Vulkan/Pipeline/GraphicsPipeLine.h"

class Renderer {
public:
    Renderer(Window& window, VkInstance instance, VkSurfaceKHR surface);
    ~Renderer();

    void drawFrame();
    void waitIdle() const { context.waitIdle(); }

private:
    void createPipeline();

    RenderContext context;
    RenderPass* renderPass = nullptr;
    PipelineLayout* pipelineLayout = nullptr;
    GraphicsPipeline* graphicsPipeline = nullptr;
    FrameManager* frameManager = nullptr;
    CommandManager* commandManager = nullptr;

    Window& window;
};