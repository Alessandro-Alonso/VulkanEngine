#include "Renderer.h"
#include "Vulkan/Platform/FileSystem.h"
#include <stdexcept>

Renderer::Renderer(Window& win, VkInstance inst, VkSurfaceKHR surf)
    : context(win, inst, surf), window(win)
{
    renderPass = new RenderPass(context.getDevice(), context.getSwapChain()->getImageFormat());
    pipelineLayout = new PipelineLayout(context.getDevice());

    createPipeline();

    frameManager = new FrameManager(context, renderPass->get(), *context.getSwapChain());

    commandManager = new CommandManager(
        context,
        renderPass->get(),
        graphicsPipeline->get(),
        *context.getSwapChain()
    );

    commandManager->recordCommands(frameManager->getFramebuffers(), *context.getSwapChain());
}

Renderer::~Renderer() {
    delete commandManager;
    delete frameManager;
    delete graphicsPipeline;
    delete pipelineLayout;
    delete renderPass;
}

void Renderer::createPipeline() {
    std::string exePath = Platform::GetExecutableDirectory();
    graphicsPipeline = new GraphicsPipeline(
        context.getDevice(),
        context.getSwapChain()->getExtent(),
        renderPass->get(),
        pipelineLayout->get(),
        (exePath + "/Vulkan/Shaders/vert.spv").c_str(),
        (exePath + "/Vulkan/Shaders/frag.spv").c_str()
    );
}

void Renderer::drawFrame() {
    // Próximo paso: sincronización
}