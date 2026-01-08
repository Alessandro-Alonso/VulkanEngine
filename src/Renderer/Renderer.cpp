#include "Renderer.h"
#include "Vulkan/Platform/FileSystem.h"
#include "Vulkan/Utils/ErrorHandling.h"
#include "Vulkan/Platform/FileSystem.h"
#include "Vulkan/Images/AllocatedImage.h"

#include <stdexcept>

namespace NETAEngine {

    void Renderer::initDescriptors() {

        VkSamplerCreateInfo samplerInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        VK_CHECK(vkCreateSampler(context.getDevice(), &samplerInfo, nullptr, &m_defaultSampler));

        // Crear diseño del conjunto des descriptores (enlace 0 = muestreador de imagenes combinado)
        VkDescriptorSetLayoutBinding binding{};
        binding.binding = 0;
        binding.descriptorCount = 1;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &binding;
        VK_CHECK(vkCreateDescriptorSetLayout(context.getDevice(), &layoutInfo, nullptr, &m_postProcessDescriptorLayout));

        // crear grupo de descriptores
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = 10;

        VkDescriptorPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = 10;
        VK_CHECK(vkCreateDescriptorPool(context.getDevice(), &poolInfo, nullptr, &m_descriptorPool));

        // Asignar el conjunto de descriptores
        VkDescriptorSetAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &m_postProcessDescriptorLayout;
        VK_CHECK(vkAllocateDescriptorSets(context.getDevice(), &allocInfo, &m_postProcessDescriptorSet));
    }

    void Renderer::updateDescriptorSets() {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = m_drawImage->getImageView();
        imageInfo.sampler = m_defaultSampler;

        VkWriteDescriptorSet descriptorWrite{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        descriptorWrite.dstSet = m_postProcessDescriptorSet;
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(context.getDevice(), 1, &descriptorWrite, 0, nullptr);
    }

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

    void Renderer::transitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout) {
        VkImageMemoryBarrier2 barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
        barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
        barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
        barrier.oldLayout = currentLayout;
        barrier.newLayout = newLayout;

        // Determinamos la mascara de aspecto automaticamente en funcion del nuevo diseño.
        VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;

        barrier.subresourceRange.aspectMask = aspectMask;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.image = image;

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

    Renderer::~Renderer() {
        VkDevice device = context.getDevice();
        vkDeviceWaitIdle(device);

        if (m_descriptorPool != VK_NULL_HANDLE) vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);
        if (m_postProcessDescriptorLayout != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(device, m_postProcessDescriptorLayout, nullptr);
        if (m_defaultSampler != VK_NULL_HANDLE) vkDestroySampler(device, m_defaultSampler, nullptr);

        // Destruir todos los renderFinishedSemaphores (uno por imagen del swapchain)
        for (VkSemaphore semaphore : renderFinishedSemaphores) {
            if (semaphore != VK_NULL_HANDLE) {
                vkDestroySemaphore(device, semaphore, nullptr);
            }
        }

        // Destruir imageAvailableSemaphores y fences (uno por frame in flight)
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }

        // Destruir command pool (libera todos los command buffers)
        if (commandPool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(device, commandPool, nullptr);
            commandPool = VK_NULL_HANDLE;
        }
    }

    void Renderer::initVulkan() {
        createImages();
        initDescriptors();

        pipelineLayout = std::make_unique<PipelineLayout>(context.getDevice(), std::vector<VkDescriptorSetLayout>{});

        m_postProcessLayout = std::make_unique<PipelineLayout>(context.getDevice(),
                              std::vector<VkDescriptorSetLayout>{ m_postProcessDescriptorLayout });

        createPipeline();
        updateDescriptorSets();

        createCommandSystem();
        createSyncObjects();
    }

    void Renderer::createPipeline() {
        std::string exePath = Filesystem::getExecutableDirectory().string();

        graphicsPipeline = std::make_unique<GraphicsPipeline>(
            context.getDevice(),
            context.getSwapChain()->getExtent(),
            m_drawImage->getFormat(),
            m_depthImage->getFormat(),
            pipelineLayout->get(),
            (exePath + "/Shaders/vert.spv").c_str(),
            (exePath + "/Shaders/frag.spv").c_str()
        );

        m_postProcessPipeline = std::make_unique<GraphicsPipeline>(
            context.getDevice(),
            context.getSwapChain()->getExtent(),
            context.getSwapChain()->getImageFormat(),
            VK_FORMAT_UNDEFINED,
            m_postProcessLayout->get(),
            (exePath + "/Shaders/fullscreen.spv").c_str(),
            (exePath + "/Shaders/tonemapping.spv").c_str()
        );
    }

    void Renderer::createCommandSystem() {
        QueueFamilyIndices queueFamilyIndices = context.getQueueFamilies();

        VkCommandPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        VK_CHECK(vkCreateCommandPool(context.getDevice(), &poolInfo, nullptr, &commandPool));

        commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        VK_CHECK(vkAllocateCommandBuffers(context.getDevice(), &allocInfo, commandBuffers.data()));

    }

    void Renderer::createSyncObjects() {
        uint32_t swapchainImageCount = context.getSwapChain()->getImageCount();

        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(swapchainImageCount);
        imagesInFlight.resize(swapchainImageCount, VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        // Creamos imageAvailable y fences (uno por frame in flight)
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            VK_CHECK(vkCreateSemaphore(context.getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]));
            VK_CHECK(vkCreateFence(context.getDevice(), &fenceInfo, nullptr, &inFlightFences[i]));
        }

        // Creamos renderFinishedSemaphores (uno por cada imagen del swapchain)
        for (uint32_t i = 0; i < swapchainImageCount; ++i) {
            VK_CHECK(vkCreateSemaphore(context.getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]));
        }
    }

    void Renderer::recordCommandBuffer(VkCommandBuffer cmd, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

        VkImage currentDrawImage = m_drawImage->getImage();
        VkImageView currentDrawImageView = m_drawImage->getImageView();
        VkExtent3D currentDrawExtent = m_drawImage->getExtent();
        VkImage currentDepthImage = m_depthImage->getImage();
        VkImageView currentDepthImageView = m_depthImage->getImageView();

        VkImage swapChainImage = context.getSwapChain()->getImage(imageIndex);
        VkImageView swapChainImageView = context.getSwapChain()->getImageView(imageIndex);
        VkExtent2D swapChainExtent = context.getSwapChain()->getExtent();

        // Off screen HDR
        transitionImage(cmd, currentDrawImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        transitionImage(cmd, currentDepthImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        // Renderiza la escena en m_drawImage (HDR)
        VkRenderingAttachmentInfo colorAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        colorAttachment.imageView = currentDrawImageView;
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue = { {{0.0f, 0.0f, 0.0f, 1.0f}} }; // Negro

        VkRenderingAttachmentInfo depthAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        depthAttachment.imageView = currentDepthImageView;
        depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.clearValue.depthStencil = { 1.0f, 0 };

        VkRenderingInfo renderingInfo{ VK_STRUCTURE_TYPE_RENDERING_INFO };
        renderingInfo.renderArea = { {0, 0}, currentDrawExtent.width, currentDrawExtent.height };
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;
        renderingInfo.pDepthAttachment = &depthAttachment;

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
        transitionImage(cmd, currentDrawImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        transitionImage(cmd, swapChainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        VkRenderingAttachmentInfo swapchainAttachment{ VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        swapchainAttachment.imageView = swapChainImageView;
        swapchainAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        swapchainAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        swapchainAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingInfo postProcessInfo{ VK_STRUCTURE_TYPE_RENDERING_INFO };
        postProcessInfo.renderArea = { {0, 0}, swapChainExtent.width, swapChainExtent.height };
        postProcessInfo.layerCount = 1;
        postProcessInfo.colorAttachmentCount = 1;
        postProcessInfo.pColorAttachments = &swapchainAttachment;

        vkCmdBeginRendering(cmd, &postProcessInfo);
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_postProcessPipeline->get());

        VkViewport postViewport{ 0.0f, 0.0f, (float)swapChainExtent.width, (float)swapChainExtent.height, 0.0f, 1.0f };
        vkCmdSetViewport(cmd, 0, 1, &postViewport);
        VkRect2D postScissor{ {0, 0}, swapChainExtent };
        vkCmdSetScissor(cmd, 0, 1, &postScissor);

        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_postProcessLayout->get(), 0, 1, &m_postProcessDescriptorSet, 0, nullptr);
        vkCmdDraw(cmd, 3, 1, 0, 0); // Dibuja el triangulo en fullscreen
        vkCmdEndRendering(cmd);

        transitionImage(cmd, swapChainImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        VK_CHECK(vkEndCommandBuffer(cmd));
    }

    void Renderer::drawFrame() {
        // Esperamos al fence del frame actual (CPU no avance mas de MAX_FRAMES_IN_FLIGHT)
        VK_CHECK(vkWaitForFences(context.getDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX));

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(context.getDevice(), context.getSwapChain()->getSwapChain(),
            UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        // Manejo de resize / out of date (lo agregare despues)
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire swap chain image!");
        }

        // Si la imagen ya esta en uso por un frame anterior, esperamos
        if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
            VK_CHECK(vkWaitForFences(context.getDevice(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX));
        }
        imagesInFlight[imageIndex] = inFlightFences[currentFrame];

        // Reseteamos el fence para este frame
        VK_CHECK(vkResetFences(context.getDevice(), 1, &inFlightFences[currentFrame]));

        // Grabamos command buffer
        recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

        // hacemos Submit
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

        // Usamos renderFinishedSemaphores[imageIndex]
        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[imageIndex] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VK_CHECK(vkQueueSubmit(context.getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]));

        // Presente
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { context.getSwapChain()->getSwapChain() };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(context.getPresentQueue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasResized()) {
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::recreateSwapChain() {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window.getWindow(), &width, &height);

        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window.getWindow(), &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(context.getDevice());

        m_drawImage.reset();
        m_depthImage.reset();

        context.recreateSwapChain();

        createImages();

        updateDescriptorSets();

        window.resetWindowResizedFlag();
    }
}
