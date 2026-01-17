#include "Renderer.h"
#include "Vulkan/Utils/ErrorHandling.h"
#include "Vulkan/Platform/FileSystem.h"
#include "Vulkan/Images/AllocatedImage.h"
#include "vulkan/vulkan_core.h"

namespace NETAEngine {

    // Esto lo que hace es crear el renderer, que es el encargado de renderizar en la ventana, y se encarga de inicializar Vulkan,
    // crear el pipeline, crear el sistema de comandos, crear los objetos de sincronizacion, crear las imagenes y los descriptores,
    // y actulizar los descriptores con la informacion de las imagenes.
    void Renderer::initDescriptors() {

        VkSamplerCreateInfo samplerInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        samplerInfo.magFilter    = VK_FILTER_LINEAR;
        samplerInfo.minFilter    = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.mipmapMode   = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        VK_CHECK(vkCreateSampler(context.getDevice(), &samplerInfo, nullptr, &m_defaultSampler));

        // Crear diseño del conjunto des descriptores (enlace 0 = muestreador de imagenes combinado)
        VkDescriptorSetLayoutBinding binding{};
        binding.binding         = 0;
        binding.descriptorCount = 1;
        binding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings    = &binding;
        VK_CHECK(vkCreateDescriptorSetLayout(context.getDevice(), &layoutInfo, nullptr, &m_postProcessDescriptorLayout));

        // crear grupo de descriptores
        VkDescriptorPoolSize poolSize{};
        poolSize.type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = DESCRIPTOR_POOL_SAMPLER_COUNT;

        VkDescriptorPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes    = &poolSize;
        poolInfo.maxSets       = DESCRIPTOR_POOL_MAX_SETS;
        poolInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        VK_CHECK(vkCreateDescriptorPool(context.getDevice(), &poolInfo, nullptr, &m_descriptorPool));

        // Asignar el conjunto de descriptores
        VkDescriptorSetAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        allocInfo.descriptorPool     = m_descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts        = &m_postProcessDescriptorLayout;
        VK_CHECK(vkAllocateDescriptorSets(context.getDevice(), &allocInfo, &m_postProcessDescriptorSet));
    }

    // Esto se encarga de actualizar los descriptores con la informacion de las imagenes, lo cual es necesario para que el shader pueda acceder a las imagenes,
    // ya que los descriptores son los que se van a usar para acceder a las imagenes en el shader, y se actualizan con la informacion de las imagenes
    // que se van a renderizar.
    void Renderer::updateDescriptorSets() {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView   = m_drawImage->getImageView();
        imageInfo.sampler     = m_defaultSampler;

        VkWriteDescriptorSet descriptorWrite{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        descriptorWrite.dstSet          = m_postProcessDescriptorSet;
        descriptorWrite.dstBinding      = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo      = &imageInfo;

        vkUpdateDescriptorSets(context.getDevice(), 1, &descriptorWrite, 0, nullptr);
    }

    // Aqui se crean las imagenes que se van a usar para renderizar, como la imagen de dibujo y la imagen de profundidad, lo cual es necesario
    // para renderizar en la ventana.
    void Renderer::createImages() {
        VkExtent2D extent = context.getSwapChain()->getExtent();
        VkExtent3D drawExtent = { extent.width, extent.height, 1 };

        // Esta mierda esta hardcodeada, cambialo despues.
        VkImageUsageFlags drawImageUsage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
            VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_STORAGE_BIT |
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT;

        m_drawImage = std::make_unique<AllocatedImage>(
            context.getDevice(),
            context.getAllocator(),
            drawExtent,
            m_drawFormat,
            drawImageUsage,
            VK_IMAGE_ASPECT_COLOR_BIT
        );

        VkImageUsageFlags depthImageUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        m_depthImage = std::make_unique<AllocatedImage>(
            context.getDevice(),
            context.getAllocator(),
            drawExtent,
            m_depthFormat,
            depthImageUsage,
            VK_IMAGE_ASPECT_DEPTH_BIT
        );

    }

    // Aqui se encarga de hacer la transicion de las imagenes entre los diferentes layouts, lo cual es necesario para renderizar en la ventana,
    // ya que las imagenes deben estar en el layout correcto para ser usadas como attachments o como texturas en el shader, y esta funcion se
    // encarga de hacer esa transicion de forma automatica, dependiendo del layout actual y el layout nuevo que se le indique.
    void Renderer::transitionImage(VkCommandBuffer cmd, VkImage image,
        VkImageLayout currentLayout, VkImageLayout newLayout,
        VkPipelineStageFlags2 srcStage, VkAccessFlags2 srcAccess,
        VkPipelineStageFlags2 dstStage, VkAccessFlags2 dstAccess) 
    {
        // Determinamos la mascara de aspecto automaticamente en funcion del nuevo dise�o.
        VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ||
                                     currentLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
                                    ? VK_IMAGE_ASPECT_DEPTH_BIT
                                    : VK_IMAGE_ASPECT_COLOR_BIT;

        VkImageMemoryBarrier2 barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
        barrier.srcStageMask                    = srcStage;
        barrier.srcAccessMask                   = srcAccess;
        barrier.dstStageMask                    = dstStage;
        barrier.dstAccessMask                   = dstAccess;
        barrier.oldLayout                       = currentLayout;
        barrier.newLayout                       = newLayout;
        barrier.image                           = image;
        barrier.subresourceRange.aspectMask     = aspectMask;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;

        VkDependencyInfo dependencyInfo{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &barrier;

        vkCmdPipelineBarrier2(cmd, &dependencyInfo);
    }

    Renderer::Renderer(Window& win, VkInstance inst, VkSurfaceKHR surf)
        : context(win, inst, surf), window(win)
    {
        initVulkan();
    }

    // Esto es basicamente el destructor del renderer, que se encarga de limpiar todos los recursos que se han creado, como los descriptores,
    // los objetos de sincronizacion, el command pool y los command buffers. Una cosa importantes que esto usando RAII, por lo tanto
    // las imagenes se limpian automaticamente cuando se destruyen los objetos de tipo AllocatedImage, y el pipeline se limpia automaticamente tambien.
    Renderer::~Renderer() {
        VkDevice device = context.getDevice();
        vkDeviceWaitIdle(device);

        m_drawImage.reset();
        m_depthImage.reset();

        if (m_descriptorPool != VK_NULL_HANDLE)
            vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);
        if (m_postProcessDescriptorLayout != VK_NULL_HANDLE)
            vkDestroyDescriptorSetLayout(device, m_postProcessDescriptorLayout, nullptr);
        if (m_defaultSampler != VK_NULL_HANDLE)
            vkDestroySampler(device, m_defaultSampler, nullptr);
        
        for (auto& frame : m_frames) {
            if (frame.commandPool != VK_NULL_HANDLE)
                vkDestroyCommandPool(device, frame.commandPool, nullptr);
            if (frame.imageAvailableSemaphore != VK_NULL_HANDLE)
                vkDestroySemaphore(device, frame.imageAvailableSemaphore, nullptr);
            if (frame.renderFinishedSemaphore != VK_NULL_HANDLE)
                vkDestroySemaphore(device, frame.renderFinishedSemaphore, nullptr);
            if (frame.inFlightFence != VK_NULL_HANDLE)
                vkDestroyFence(device, frame.inFlightFence, nullptr);
        }    
        
        for (VkSemaphore semaphore : m_renderFinishedSemaphores) {
            if (semaphore != VK_NULL_HANDLE)
            vkDestroySemaphore(device, semaphore, nullptr);
        }
        
        if (transferCommandPool != VK_NULL_HANDLE)
            vkDestroyCommandPool(device, transferCommandPool, nullptr);

    }

    // Esto lo que hace es crear el renderer, que es el encargado de renderizar en la ventana, y se encarga de inicializar Vulkan,
    // crear el pipeline, crear el sistema de comandos, crear los objetos de sincronizacion, crear las imagenes y los descriptores.
    void Renderer::initVulkan() {
        createImages();
        initDescriptors();

        pipelineLayout = std::make_unique<PipelineLayout>(context.getDevice(), std::vector<VkDescriptorSetLayout>{});

        m_postProcessLayout = std::make_unique<PipelineLayout>(context.getDevice(),
                              std::vector<VkDescriptorSetLayout>{ m_postProcessDescriptorLayout });

        createPipeline();
        updateDescriptorSets();
        initFrameData();
    }

    // Esto es muy simple. Se encarga de crear el pipeline grafico, que es el encargado de renderizar la escena en la ventana,
    // y se le pasan los shaders de vertice y fragmento, que son los que se van a usar para renderizar la escena.
    void Renderer::createPipeline() {
        std::string exePath = Filesystem::getExecutableDirectory().string();

        graphicsPipeline = std::make_unique<GraphicsPipeline>(
            context.getDevice(),
            m_drawImage->getFormat(),
            m_depthImage->getFormat(),
            pipelineLayout->get(),
            (exePath + "/Shaders/vert.spv").c_str(),
            (exePath + "/Shaders/frag.spv").c_str()
        );

        m_postProcessPipeline = std::make_unique<GraphicsPipeline>(
            context.getDevice(),
            context.getSwapChain()->getImageFormat(),
            VK_FORMAT_UNDEFINED,
            m_postProcessLayout->get(),
            (exePath + "/Shaders/fullscreen.spv").c_str(),
            (exePath + "/Shaders/tonemapping.spv").c_str()
        );
    }

    void Renderer::initFrameData() {
        uint32_t graphicsFamily = context.getQueueFamilies().graphicsFamily.value();

        VkCommandPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = graphicsFamily;

        VkCommandBufferAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkSemaphoreCreateInfo semaphoreInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (auto& frame : m_frames) {
            VK_CHECK(vkCreateCommandPool(context.getDevice(), &poolInfo, nullptr, &frame.commandPool));

            allocInfo.commandPool = frame.commandPool;
            VK_CHECK(vkAllocateCommandBuffers(context.getDevice(), &allocInfo, &frame.commandBuffer));

            VK_CHECK(vkCreateSemaphore(context.getDevice(), &semaphoreInfo, nullptr, &frame.imageAvailableSemaphore));
            VK_CHECK(vkCreateFence(context.getDevice(), &fenceInfo, nullptr, &frame.inFlightFence));
        }

        uint32_t imageCount = context.getSwapChain()->getImageCount();
        m_renderFinishedSemaphores.resize(imageCount);

        for (uint32_t i = 0; i < imageCount; i++) {
            VK_CHECK(vkCreateSemaphore(context.getDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]));
        }

        VkCommandPoolCreateInfo transferPoolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        transferPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        transferPoolInfo.queueFamilyIndex = graphicsFamily;
        VK_CHECK(vkCreateCommandPool(context.getDevice(), &transferPoolInfo, nullptr, &transferCommandPool));
    }

    // Esto se encarga de grabar los comandos de renderizado en el command buffer, lo cual es necesario para renderizar en la ventana.
    void Renderer::recordCommandBuffer(VkCommandBuffer cmd, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

        VkImage currentDrawImage          = m_drawImage->getImage();
        VkImageView currentDrawImageView  = m_drawImage->getImageView();
        VkExtent3D currentDrawExtent      = m_drawImage->getExtent();
        VkImage currentDepthImage         = m_depthImage->getImage();
        VkImageView currentDepthImageView = m_depthImage->getImageView();

        VkImage swapChainImage            = context.getSwapChain()->getImage(imageIndex);
        VkImageView swapChainImageView    = context.getSwapChain()->getImageView(imageIndex);
        VkExtent2D swapChainExtent        = context.getSwapChain()->getExtent();

        // Off screen HDR
        transitionImage(cmd, currentDrawImage,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT, VK_ACCESS_2_NONE,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);
        // CORRECT:
        transitionImage(cmd, currentDepthImage,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,       VK_ACCESS_2_NONE,
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

        // Renderiza la escena en m_drawImage (HDR)
        VkRenderingAttachmentInfo colorAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        colorAttachment.imageView   = currentDrawImageView;
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue  = { {{0.0f, 0.0f, 0.0f, 1.0f}} }; // Negro

        VkRenderingAttachmentInfo depthAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        depthAttachment.imageView               = currentDepthImageView;
        depthAttachment.imageLayout             = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        depthAttachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.clearValue.depthStencil = { 1.0f, 0 };

        VkRenderingInfo renderingInfo{ VK_STRUCTURE_TYPE_RENDERING_INFO };
        renderingInfo.renderArea           = { {0, 0}, currentDrawExtent.width, currentDrawExtent.height };
        renderingInfo.layerCount           = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments    = &colorAttachment;
        renderingInfo.pDepthAttachment     = &depthAttachment;

        // Empieza el dynamic render
        vkCmdBeginRendering(cmd, &renderingInfo);
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->get());

        // Setup del estado dynamico
        VkViewport viewport{ 0.0f, 0.0f, (float)currentDrawExtent.width, (float)currentDrawExtent.height, 0.0f, 1.0f };
        vkCmdSetViewport(cmd, 0, 1, &viewport);
        VkRect2D scissor{ {0, 0}, {currentDrawExtent.width, currentDrawExtent.height} };
        vkCmdSetScissor(cmd, 0, 1, &scissor);

        // Dibuja
        vkCmdDraw(cmd, 3, 1, 0, 0);

        // Terminar el render
        vkCmdEndRendering(cmd);

        // Transiciona las imagenes al presente
        transitionImage(cmd, currentDrawImage,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT);
        transitionImage(cmd, swapChainImage,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT, VK_ACCESS_2_NONE,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

        VkRenderingAttachmentInfo swapchainAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        swapchainAttachment.imageView   = swapChainImageView;
        swapchainAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        swapchainAttachment.loadOp      = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        swapchainAttachment.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingInfo postProcessInfo{ VK_STRUCTURE_TYPE_RENDERING_INFO };
        postProcessInfo.renderArea           = { {0, 0}, swapChainExtent.width, swapChainExtent.height };
        postProcessInfo.layerCount           = 1;
        postProcessInfo.colorAttachmentCount = 1;
        postProcessInfo.pColorAttachments    = &swapchainAttachment;

        vkCmdBeginRendering(cmd, &postProcessInfo);
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_postProcessPipeline->get());

        VkViewport postViewport{ 0.0f, 0.0f, (float)swapChainExtent.width, (float)swapChainExtent.height, 0.0f, 1.0f };
        vkCmdSetViewport(cmd, 0, 1, &postViewport);
        VkRect2D postScissor{ {0, 0}, swapChainExtent };
        vkCmdSetScissor(cmd, 0, 1, &postScissor);

        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_postProcessLayout->get(), 0, 1, &m_postProcessDescriptorSet, 0, nullptr);
        vkCmdDraw(cmd, 3, 1, 0, 0); // Dibuja el triangulo en fullscreen
        vkCmdEndRendering(cmd);

        transitionImage(cmd, swapChainImage,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT, VK_ACCESS_2_NONE);
        VK_CHECK(vkEndCommandBuffer(cmd));
    }

    // Dibujar un frame. Esto se encarga de esperar a que el frame anterior termine de adquirir la imagen, grabar los comandos de renderizado en el command bufffer,
    // hacer submit del command buffer a la cola de graficos, y presentar la imagen en la ventana.
    void Renderer::drawFrame() {
        FrameData& frame = getCurrentFrame();
        VkDevice device = context.getDevice();

        VK_CHECK(vkWaitForFences(device, 1, &frame.inFlightFence, VK_TRUE, UINT64_MAX));
        
        uint32_t imageIndex;
        auto acquireResult = VK_CALL(vkAcquireNextImageKHR(
            device,
            context.getSwapChain()->getSwapChain(),
            UINT64_MAX,
            frame.imageAvailableSemaphore,
            VK_NULL_HANDLE,
            &imageIndex));
        
        if (!acquireResult.ok()) {
            if (acquireResult.isRecoverable()) {
                recreateSwapChain();
                return;
            }
            logAndAbort(acquireResult);
        }

        VK_CHECK(vkResetFences(device, 1, &frame.inFlightFence));
        VK_CHECK(vkResetCommandBuffer(frame.commandBuffer, 0));

        recordCommandBuffer(frame.commandBuffer, imageIndex);

        VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = &frame.imageAvailableSemaphore;
        submitInfo.pWaitDstStageMask    = &waitStages;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &frame.commandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = &m_renderFinishedSemaphores[imageIndex];

        VK_CHECK(vkQueueSubmit(context.getGraphicsQueue(), 1, &submitInfo, frame.inFlightFence));

        VkSwapchainKHR swapChains = context.getSwapChain()->getSwapChain();

        VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = &m_renderFinishedSemaphores[imageIndex];
        presentInfo.swapchainCount     = 1;
        presentInfo.pSwapchains        = &swapChains;
        presentInfo.pImageIndices      = &imageIndex;

        auto presentResult = VK_CALL(vkQueuePresentKHR(context.getPresentQueue(), &presentInfo));

        if (!presentResult.ok() || window.wasResized()) {
            window.resetWindowResizedFlag();
            if (presentResult.isFatal()) {
                logAndAbort(presentResult);
            }
            recreateSwapChain();
        }

        m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::recreateSwapChain() {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window.getWindow(), &width, &height);

        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window.getWindow(), &width, &height);
            glfwWaitEvents();
        }

        std::vector<VkFence> fences;
        for (const auto& frame : m_frames) {
            fences.push_back(frame.inFlightFence);
        }

        vkWaitForFences(context.getDevice(), static_cast<uint32_t>(fences.size()), fences.data(), VK_TRUE, UINT64_MAX);

        context.recreateSwapChain();

        createImages();
        updateDescriptorSets();

        m_currentFrame = 0;
    }
}
