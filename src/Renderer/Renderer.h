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

    std::unique_ptr<RenderPass> renderPass;
    std::unique_ptr<PipelineLayout> pipelineLayout;
    std::unique_ptr<GraphicsPipeline> graphicsPipeline;
    std::unique_ptr<FrameManager> frameManager;
    std::unique_ptr<CommandManager> commandManager;

    Window& window;
};