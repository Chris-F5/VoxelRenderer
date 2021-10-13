#include "graphics_pipeline.h"

#include <string.h>

#include <vulkan/vulkan_core.h>

#include "scene_data/debug_line.h"
#include "device.h"
#include "scene_data/mesh_gen.h"
#include "vk_utils/exceptions.h"

VkPipelineShaderStageCreateInfo createShaderStage(
    VkShaderModule module,
    VkShaderStageFlagBits stage)
{
    VkPipelineShaderStageCreateInfo shaderStage;
    shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.pNext = NULL;
    shaderStage.flags = 0;
    shaderStage.stage = stage;
    shaderStage.module = module;
    shaderStage.pName = "main";
    shaderStage.pSpecializationInfo = NULL;

    return shaderStage;
}

void createGraphicsPipelines(
    VkDevice device,
    PhysicalDeviceProperties physicalDeviceProperties,
    Swapchain swapchain,

    VkShaderModule mainPipelineVertShader,
    VkShaderModule mainPipelineFragShader,
    uint32_t mainPipelineDescriptorSetLayoutCount,
    const VkDescriptorSetLayout* mainPipelineDescriptorSetLayouts,

    VkShaderModule debugLinePipelineVertShader,
    VkShaderModule debugLinePipelineFragShader,
    uint32_t debugLinePipelineDescriptorSetLayoutCount,
    const VkDescriptorSetLayout* debugLinePipelineDescriptorSetLayouts,

    VkPipelineLayout* mainPipelineLayout,
    VkPipeline* mainPipeline,
    VkPipelineLayout* debugLinePipelineLayout,
    VkPipeline* debugLinePipeline,
    VkRenderPass* renderPass)
{
    // RENDER PASS
    {
        // RENDER PASS ATTACHMENTS

        VkAttachmentDescription colorAttachment;
        colorAttachment.flags = 0;
        colorAttachment.format = swapchain.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef;
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment;
        depthAttachment.flags = 0;
        depthAttachment.format = physicalDeviceProperties.depthBufferFormat;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAtttachmentRef;
        depthAtttachmentRef.attachment = 1;
        depthAtttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // MAIN SUBPASS

        VkSubpassDescription mainSubpass;
        mainSubpass.flags = 0;
        mainSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        mainSubpass.inputAttachmentCount = 0;
        mainSubpass.pInputAttachments = NULL;
        mainSubpass.colorAttachmentCount = 1;
        mainSubpass.pColorAttachments = &colorAttachmentRef;
        mainSubpass.pResolveAttachments = NULL;
        mainSubpass.pDepthStencilAttachment = &depthAtttachmentRef;
        mainSubpass.preserveAttachmentCount = 0;
        mainSubpass.pPreserveAttachments = NULL;

        // DEBUG LINE SUBPASS

        VkSubpassDescription debugLineSubpass;
        debugLineSubpass.flags = 0;
        debugLineSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        debugLineSubpass.inputAttachmentCount = 0;
        debugLineSubpass.pInputAttachments = NULL;
        debugLineSubpass.colorAttachmentCount = 1;
        debugLineSubpass.pColorAttachments = &colorAttachmentRef;
        debugLineSubpass.pResolveAttachments = NULL;
        debugLineSubpass.pDepthStencilAttachment = &depthAtttachmentRef;
        debugLineSubpass.preserveAttachmentCount = 0;
        debugLineSubpass.pPreserveAttachments = NULL;

        // SUBPASS DEPENDENCIES

        VkSubpassDependency mainSubpassDependency;
        mainSubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        mainSubpassDependency.dstSubpass = 0;
        mainSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        mainSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        mainSubpassDependency.srcAccessMask = 0;
        mainSubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        mainSubpassDependency.dependencyFlags = 0;

        VkSubpassDependency debugLineSubpassDependency;
        debugLineSubpassDependency.srcSubpass = 0;
        debugLineSubpassDependency.dstSubpass = 1;
        debugLineSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        debugLineSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        debugLineSubpassDependency.srcAccessMask = 0;
        debugLineSubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        debugLineSubpassDependency.dependencyFlags = 0;

        // RENDER PASS
        VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };
        VkSubpassDescription subpasses[] = { mainSubpass, debugLineSubpass };
        VkSubpassDependency subpassDependencies[] = { mainSubpassDependency, debugLineSubpassDependency };

        VkRenderPassCreateInfo renderPassCreateInfo;
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.pNext = NULL;
        renderPassCreateInfo.flags = 0;
        renderPassCreateInfo.attachmentCount = sizeof(attachments) / sizeof(attachments[0]);
        renderPassCreateInfo.pAttachments = attachments;
        renderPassCreateInfo.subpassCount = sizeof(subpasses) / sizeof(subpasses[0]);
        renderPassCreateInfo.pSubpasses = subpasses;
        renderPassCreateInfo.dependencyCount = sizeof(subpassDependencies) / sizeof(subpassDependencies[0]);
        renderPassCreateInfo.pDependencies = subpassDependencies;

        handleVkResult(
            vkCreateRenderPass(device, &renderPassCreateInfo, NULL, renderPass),
            "creating render pass");
    }

    // MAIN PIPELINE
    {
        // PIPELINE LAYOUT

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext = NULL;
        pipelineLayoutCreateInfo.flags = 0;
        pipelineLayoutCreateInfo.setLayoutCount = mainPipelineDescriptorSetLayoutCount;
        pipelineLayoutCreateInfo.pSetLayouts = mainPipelineDescriptorSetLayouts;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = NULL;

        handleVkResult(
            vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, NULL, mainPipelineLayout),
            "creating main graphics pipeline layout");
        // SHADER STAGES

        VkPipelineShaderStageCreateInfo vertShaderStage = createShaderStage(mainPipelineVertShader, VK_SHADER_STAGE_VERTEX_BIT);
        VkPipelineShaderStageCreateInfo fragShaderStage = createShaderStage(mainPipelineFragShader, VK_SHADER_STAGE_FRAGMENT_BIT);

        // VERTEX INPUT

        VkPipelineVertexInputStateCreateInfo vertexInputInfo;
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.pNext = NULL;
        vertexInputInfo.flags = 0;
        vertexInputInfo.vertexBindingDescriptionCount = MODEL_VERTEX_BINDING_DESCRIPTION_COUNT;
        vertexInputInfo.pVertexBindingDescriptions = MODEL_VERTEX_BINDING_DESCRIPTIONS;
        vertexInputInfo.vertexAttributeDescriptionCount = MODEL_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT;
        vertexInputInfo.pVertexAttributeDescriptions = MODEL_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS;

        // INPUT ASSEMBLY

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.pNext = NULL;
        inputAssemblyInfo.flags = 0;
        inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        // VIEWPORT

        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapchain.extent.width;
        viewport.height = (float)swapchain.extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor;
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent = swapchain.extent;

        VkPipelineViewportStateCreateInfo viewportInfo;
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.pNext = NULL;
        viewportInfo.flags = 0;
        viewportInfo.viewportCount = 1;
        viewportInfo.pViewports = &viewport;
        viewportInfo.scissorCount = 1;
        viewportInfo.pScissors = &scissor;

        // RASTERIZATION

        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationInfo.pNext = NULL;
        rasterizationInfo.flags = 0;
        rasterizationInfo.depthClampEnable = VK_FALSE;
        rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationInfo.depthBiasEnable = VK_FALSE;
        rasterizationInfo.depthBiasConstantFactor = 0.0f;
        rasterizationInfo.depthBiasClamp = 0.0f;
        rasterizationInfo.depthBiasSlopeFactor = 0.0f;
        rasterizationInfo.lineWidth = 1.0f;

        // MULTISAMPLING

        VkPipelineMultisampleStateCreateInfo multisamplingInfo;
        multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisamplingInfo.pNext = NULL;
        multisamplingInfo.flags = 0;
        multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisamplingInfo.sampleShadingEnable = VK_FALSE;
        multisamplingInfo.minSampleShading = 0.0f;
        multisamplingInfo.pSampleMask = NULL;
        multisamplingInfo.alphaToCoverageEnable = VK_FALSE;
        multisamplingInfo.alphaToOneEnable = VK_FALSE;

        // DEPTH STENCIL

        VkPipelineDepthStencilStateCreateInfo depthStencil;
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.pNext = NULL;
        depthStencil.flags = 0;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;
        memset(&depthStencil.front, 0, sizeof(VkStencilOpState));
        memset(&depthStencil.back, 0, sizeof(VkStencilOpState));
        depthStencil.minDepthBounds = 0.0f;
        depthStencil.maxDepthBounds = 1.0f;

        // COLOR BLENDING

        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.pNext = NULL;
        colorBlendInfo.flags = 0;
        colorBlendInfo.logicOpEnable = VK_FALSE;
        colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
        colorBlendInfo.attachmentCount = 1;
        colorBlendInfo.pAttachments = &colorBlendAttachment;
        colorBlendInfo.blendConstants[0] = 0.0f;
        colorBlendInfo.blendConstants[1] = 0.0f;
        colorBlendInfo.blendConstants[2] = 0.0f;
        colorBlendInfo.blendConstants[3] = 0.0f;

        // GRAPHICS PIPELINE

        VkGraphicsPipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.pNext = NULL;
        pipelineCreateInfo.flags = 0;
        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStage, fragShaderStage };
        pipelineCreateInfo.stageCount = sizeof(shaderStages) / sizeof(shaderStages[0]);
        pipelineCreateInfo.pStages = shaderStages;
        pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
        pipelineCreateInfo.pTessellationState = NULL;
        pipelineCreateInfo.pViewportState = &viewportInfo;
        pipelineCreateInfo.pRasterizationState = &rasterizationInfo;
        pipelineCreateInfo.pMultisampleState = &multisamplingInfo;
        pipelineCreateInfo.pDepthStencilState = &depthStencil;
        pipelineCreateInfo.pColorBlendState = &colorBlendInfo;
        pipelineCreateInfo.pDynamicState = NULL;
        pipelineCreateInfo.layout = *mainPipelineLayout;
        pipelineCreateInfo.renderPass = *renderPass;
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = 0;

        handleVkResult(
            vkCreateGraphicsPipelines(
                device,
                VK_NULL_HANDLE,
                1,
                &pipelineCreateInfo,
                NULL,
                mainPipeline),
            "creating main graphics pipeline");
    }

    // DEBUG LINE PIPELINE
    {
        // PIPELINE LAYOUT

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext = NULL;
        pipelineLayoutCreateInfo.flags = 0;
        pipelineLayoutCreateInfo.setLayoutCount = debugLinePipelineDescriptorSetLayoutCount;
        pipelineLayoutCreateInfo.pSetLayouts = debugLinePipelineDescriptorSetLayouts;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = NULL;

        handleVkResult(
            vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, NULL, debugLinePipelineLayout),
            "creating debug line graphics pipeline layout");

        // SHADER STAGES

        VkPipelineShaderStageCreateInfo vertShaderStage = createShaderStage(debugLinePipelineVertShader, VK_SHADER_STAGE_VERTEX_BIT);
        VkPipelineShaderStageCreateInfo fragShaderStage = createShaderStage(debugLinePipelineFragShader, VK_SHADER_STAGE_FRAGMENT_BIT);

        // VERTEX INPUT

        VkPipelineVertexInputStateCreateInfo vertexInputInfo;
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.pNext = NULL;
        vertexInputInfo.flags = 0;
        vertexInputInfo.vertexBindingDescriptionCount = DEBUG_LINE_VERTEX_BINDING_DESCRIPTION_COUNT;
        vertexInputInfo.pVertexBindingDescriptions = DEBUG_LINE_VERTEX_BINDING_DESCRIPTIONS;
        vertexInputInfo.vertexAttributeDescriptionCount = DEBUG_LINE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT;
        vertexInputInfo.pVertexAttributeDescriptions = DEBUG_LINE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS;

        // INPUT ASSEMBLY

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.pNext = NULL;
        inputAssemblyInfo.flags = 0;
        inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        // VIEWPORT

        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapchain.extent.width;
        viewport.height = (float)swapchain.extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor;
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent = swapchain.extent;

        VkPipelineViewportStateCreateInfo viewportInfo;
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.pNext = NULL;
        viewportInfo.flags = 0;
        viewportInfo.viewportCount = 1;
        viewportInfo.pViewports = &viewport;
        viewportInfo.scissorCount = 1;
        viewportInfo.pScissors = &scissor;

        // RASTERIZATION

        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationInfo.pNext = NULL;
        rasterizationInfo.flags = 0;
        rasterizationInfo.depthClampEnable = VK_FALSE;
        rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationInfo.depthBiasEnable = VK_FALSE;
        rasterizationInfo.depthBiasConstantFactor = 0.0f;
        rasterizationInfo.depthBiasClamp = 0.0f;
        rasterizationInfo.depthBiasSlopeFactor = 0.0f;
        rasterizationInfo.lineWidth = 1.0f;

        // MULTISAMPLING

        VkPipelineMultisampleStateCreateInfo multisamplingInfo;
        multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisamplingInfo.pNext = NULL;
        multisamplingInfo.flags = 0;
        multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisamplingInfo.sampleShadingEnable = VK_FALSE;
        multisamplingInfo.minSampleShading = 0.0f;
        multisamplingInfo.pSampleMask = NULL;
        multisamplingInfo.alphaToCoverageEnable = VK_FALSE;
        multisamplingInfo.alphaToOneEnable = VK_FALSE;

        // DEPTH STENCIL

        VkPipelineDepthStencilStateCreateInfo depthStencil;
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.pNext = NULL;
        depthStencil.flags = 0;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;
        memset(&depthStencil.front, 0, sizeof(VkStencilOpState));
        memset(&depthStencil.back, 0, sizeof(VkStencilOpState));
        depthStencil.minDepthBounds = 0.0f;
        depthStencil.maxDepthBounds = 1.0f;

        // COLOR BLENDING

        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.pNext = NULL;
        colorBlendInfo.flags = 0;
        colorBlendInfo.logicOpEnable = VK_FALSE;
        colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
        colorBlendInfo.attachmentCount = 1;
        colorBlendInfo.pAttachments = &colorBlendAttachment;
        colorBlendInfo.blendConstants[0] = 0.0f;
        colorBlendInfo.blendConstants[1] = 0.0f;
        colorBlendInfo.blendConstants[2] = 0.0f;
        colorBlendInfo.blendConstants[3] = 0.0f;

        // GRAPHICS PIPELINE

        VkGraphicsPipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.pNext = NULL;
        pipelineCreateInfo.flags = 0;
        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStage, fragShaderStage };
        pipelineCreateInfo.stageCount = sizeof(shaderStages) / sizeof(shaderStages[0]);
        pipelineCreateInfo.pStages = shaderStages;
        pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
        pipelineCreateInfo.pTessellationState = NULL;
        pipelineCreateInfo.pViewportState = &viewportInfo;
        pipelineCreateInfo.pRasterizationState = &rasterizationInfo;
        pipelineCreateInfo.pMultisampleState = &multisamplingInfo;
        pipelineCreateInfo.pDepthStencilState = &depthStencil;
        pipelineCreateInfo.pColorBlendState = &colorBlendInfo;
        pipelineCreateInfo.pDynamicState = NULL;
        pipelineCreateInfo.layout = *debugLinePipelineLayout;
        pipelineCreateInfo.renderPass = *renderPass;
        pipelineCreateInfo.subpass = 1;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = 0;

        handleVkResult(
            vkCreateGraphicsPipelines(
                device,
                VK_NULL_HANDLE,
                1,
                &pipelineCreateInfo,
                NULL,
                debugLinePipeline),
            "creating graphics pipeline");
    }
}
