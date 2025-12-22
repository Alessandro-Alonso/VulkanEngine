#include "Renderer.h"
#include "Vulkan/Platform/FileSystem.h"
#include <stdexcept>

Renderer::Renderer(Window& win, VkInstance inst, VkSurfaceKHR surf)
    : context(win, inst, surf), window(win)
{
    renderPass = std::make_unique<RenderPass>(context.getDevice(), context.getSwapChain()->getImageFormat());
    pipelineLayout = std::make_unique<PipelineLayout>(context.getDevice());

    createPipeline();

    frameManager = std::make_unique<FrameManager>(context, renderPass->get(), *context.getSwapChain());

    commandManager = std::make_unique<CommandManager>(
        context,
        renderPass->get(),
        graphicsPipeline->get(),
        *context.getSwapChain()
    );

    commandManager->recordCommands(frameManager->getFramebuffers(), *context.getSwapChain());
}

Renderer::~Renderer() {

    context.waitIdle();
}

void Renderer::createPipeline() {
    std::string exePath = Platform::GetExecutableDirectory();
    graphicsPipeline = std::make_unique<GraphicsPipeline>(
        context.getDevice(),
        context.getSwapChain()->getExtent(),
        renderPass->get(),
        pipelineLayout->get(),
        (exePath + "/Vulkan/Shaders/vert.spv").c_str(),
        (exePath + "/Vulkan/Shaders/frag.spv").c_str()
    );
}

void Renderer::drawFrame() {
    // Proximo paso: sincronizacion
}