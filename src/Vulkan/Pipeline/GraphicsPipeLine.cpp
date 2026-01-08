#include "GraphicsPipeLine.h"
#include "Vulkan/Shaders/ShaderModule.h"
#include "Vulkan/Utils/ErrorHandling.h"
#include <vulkan/vulkan.h>
#include "Vulkan/Shaders/ShaderModule.h"

GraphicsPipeline::GraphicsPipeline(
    VkDevice device,
    VkExtent2D extent,
    VkFormat colorAttachmentFormat,
    VkFormat depthAttachmentFormat,
    VkPipelineLayout pipelineLayout,
    const char* vertFile,
    const char* fragFile)
    : device(device),
    vertexShaderModule(device, vertFile),
    fragmentShaderModule(device, fragFile)
{
    VkPipelineShaderStageCreateInfo vertStage{};
    vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStage.module = vertexShaderModule.get();
    vertStage.pName = "main";

    VkPipelineShaderStageCreateInfo fragStage{};
    fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStage.module = fragmentShaderModule.get();
    fragStage.pName = "main";

    VkPipelineShaderStageCreateInfo stages[] = { vertStage, fragStage };

    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    VkRect2D scissor{};
    scissor.offset = { 0,0 };
    scissor.extent = extent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // Rasterizacion
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.lineWidth = 1.0f;

    if (depthAttachmentFormat == VK_FORMAT_UNDEFINED) {
        rasterizer.cullMode = VK_CULL_MODE_NONE;
    }
    else {
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    }

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState blendAttachment{};
    blendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    blendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo blending{};
    blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blending.attachmentCount = 1;
    blending.pAttachments = &blendAttachment;

    // Estado dinamico
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // Estado del depth stencil
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    // Desactiva el depth test si el formate no es conocido (Post-process)
    if (depthAttachmentFormat == VK_FORMAT_UNDEFINED) {
        depthStencil.depthTestEnable = VK_FALSE;
        depthStencil.depthWriteEnable = VK_FALSE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;
    }
    else {
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    }

    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineRenderingCreateInfo renderingCreateInfo{};
    renderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    renderingCreateInfo.colorAttachmentCount = 1;
    renderingCreateInfo.pColorAttachmentFormats = &colorAttachmentFormat;
    renderingCreateInfo.depthAttachmentFormat = depthAttachmentFormat;

    // Creacion final del proceso
    VkGraphicsPipelineCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    info.pInputAssemblyState = &inputAssembly;
    info.pMultisampleState = &multisampling;
    info.pDepthStencilState = &depthStencil;
    info.pRasterizationState = &rasterizer;
    info.pVertexInputState = &vertexInput;
    info.pViewportState = &viewportState;
    info.pDynamicState = &dynamicState;
    info.pColorBlendState = &blending;
    info.pNext = &renderingCreateInfo;
    info.renderPass = VK_NULL_HANDLE;
    info.layout = pipelineLayout;
    info.pStages = stages;
    info.stageCount = 2;
    
    // Error Handling.
    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline));
}

GraphicsPipeline::~GraphicsPipeline() {
    vkDestroyPipeline(device, pipeline, nullptr);
}