#include "./renderer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "vk_utils/buffer.h"
#include "vk_utils/descriptor_set.h"
#include "vk_utils/exceptions.h"
#include "vk_utils/image.h"
#include "vk_utils/shader_module.h"

#include "./utils.h"

static void createSwapchain(
    VkDevice logicalDevice,
    VkSurfaceKHR surface,
    VkSurfaceFormatKHR surfaceFormat,
    VkSurfaceCapabilitiesKHR surfaceCapabilities,
    VkPresentModeKHR presentMode,
    uint32_t graphicsFamilyIndex,
    uint32_t presentFamilyIndex,
    VkExtent2D extent,
    VkSwapchainKHR* swapchain)
{
    uint32_t minSwapLen = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount != 0)
        minSwapLen = min(
            minSwapLen,
            surfaceCapabilities.maxImageCount);

    VkSwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext = NULL;
    swapchainCreateInfo.flags = 0;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = minSwapLen;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (graphicsFamilyIndex != presentFamilyIndex) {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        uint32_t queueFamilyIndices[] = { graphicsFamilyIndex, presentFamilyIndex };
        swapchainCreateInfo.queueFamilyIndexCount = sizeof(queueFamilyIndices) / sizeof(queueFamilyIndices[0]);
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = NULL;
    }
    swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    handleVkResult(
        vkCreateSwapchainKHR(
            logicalDevice,
            &swapchainCreateInfo,
            NULL,
            swapchain),
        "creating swapchain");
}

static void createDepthImage(
    VkDevice logicalDevice,
    VkPhysicalDevice physicalDevice,
    VkFormat depthImageFormat,
    VkExtent2D extent,
    VkImage* depthImage,
    VkDeviceMemory* depthImageMemory,
    VkImageView* depthImageView)
{
    VkExtent3D extent3D;
    extent3D.width = extent.width;
    extent3D.height = extent.height;
    extent3D.depth = 1;

    createImage(
        logicalDevice,
        physicalDevice,
        VK_IMAGE_TYPE_2D,
        depthImageFormat,
        extent3D,
        0,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        1,
        1,
        VK_SAMPLE_COUNT_1_BIT,
        VK_IMAGE_TILING_OPTIMAL,
        false,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depthImage,
        depthImageMemory);

    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    createImageView(
        logicalDevice,
        *depthImage,
        depthImageFormat,
        VK_IMAGE_VIEW_TYPE_2D,
        subresourceRange,
        depthImageView);
}

static void createCameraDescriptorSetLayout(
    VkDevice logicalDevice,
    VkDescriptorSetLayout* layout)
{
    VkDescriptorSetLayoutBinding uboBinding;
    uboBinding.binding = 0;
    uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboBinding.descriptorCount = 1;
    uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboBinding.pImmutableSamplers = NULL;

    VkDescriptorSetLayoutCreateInfo layoutCreateInfo;
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreateInfo.pNext = NULL;
    layoutCreateInfo.flags = 0;
    layoutCreateInfo.bindingCount = 1;
    layoutCreateInfo.pBindings = &uboBinding;

    handleVkResult(
        vkCreateDescriptorSetLayout(
            logicalDevice,
            &layoutCreateInfo,
            NULL,
            layout),
        "creating camera descriptor set layout");
}

static void createCameraDescriptorPool(
    VkDevice logicalDevice,
    uint32_t setCount,
    VkDescriptorPool* pool)
{
    VkDescriptorPoolSize poolSize;
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = setCount;

    VkDescriptorPoolCreateInfo poolCreateInfo;
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.pNext = NULL;
    poolCreateInfo.flags = 0;
    poolCreateInfo.maxSets = setCount;
    poolCreateInfo.poolSizeCount = 1;
    poolCreateInfo.pPoolSizes = &poolSize;

    handleVkResult(
        vkCreateDescriptorPool(
            logicalDevice,
            &poolCreateInfo,
            NULL,
            pool),
        "creating camera descriptor pool");
}

static void createRenderPass(
    VkDevice logicalDevice,
    VkFormat swapImageFormat,
    VkFormat depthImageFormat,
    VkRenderPass* renderPass)
{
    /* RENDER PASS ATTACHMENTS */
    VkAttachmentDescription colorAttachment;
    colorAttachment.flags = 0;
    colorAttachment.format = swapImageFormat;
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
    depthAttachment.format = depthImageFormat;
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

    /* VOX TRI SUBPASS */
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

    /* DEBUG LINE SUBPASS */
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

    /* SUBPASS DEPENDENCIES */
    VkSubpassDependency mainSubpassDependency;
    mainSubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    mainSubpassDependency.dstSubpass = 0;
    mainSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    mainSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    mainSubpassDependency.srcAccessMask = 0;
    mainSubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    mainSubpassDependency.dependencyFlags = 0;

    VkSubpassDependency debugLineSubpassDependency;
    debugLineSubpassDependency.srcSubpass = 0;
    debugLineSubpassDependency.dstSubpass = 1;
    debugLineSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    debugLineSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    debugLineSubpassDependency.srcAccessMask = 0;
    debugLineSubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    debugLineSubpassDependency.dependencyFlags = 0;

    /* RENDER PASS */
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
        vkCreateRenderPass(
            logicalDevice,
            &renderPassCreateInfo,
            NULL,
            renderPass),
        "creating render pass");
}

static void createVoxTriPipeline(
    VkDevice logicalDevice,
    VkPipelineLayout layout,
    VkRenderPass renderPass,
    VkExtent2D presentExtent,
    VkShaderModule vertShaderModule,
    VkShaderModule fragShaderModule,
    VkPipeline* pipeline)
{
    /* SHADER STAGES */
    VkPipelineShaderStageCreateInfo vertShaderStage;
    vertShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStage.pNext = NULL;
    vertShaderStage.flags = 0;
    vertShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStage.module = vertShaderModule;
    vertShaderStage.pName = "main";
    vertShaderStage.pSpecializationInfo = NULL;
    VkPipelineShaderStageCreateInfo fragShaderStage;
    fragShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStage.pNext = NULL;
    fragShaderStage.flags = 0;
    fragShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStage.module = fragShaderModule;
    fragShaderStage.pName = "main";
    fragShaderStage.pSpecializationInfo = NULL;

    /* VERTEX INPUT */

    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = NULL;
    vertexInputInfo.flags = 0;
    vertexInputInfo.vertexBindingDescriptionCount = MODEL_VERTEX_BINDING_DESCRIPTION_COUNT;
    vertexInputInfo.pVertexBindingDescriptions = MODEL_VERTEX_BINDING_DESCRIPTIONS;
    vertexInputInfo.vertexAttributeDescriptionCount = MODEL_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT;
    vertexInputInfo.pVertexAttributeDescriptions = MODEL_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS;

    /* INPUT ASSEMBLY */

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.pNext = NULL;
    inputAssemblyInfo.flags = 0;
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    /* VIEWPORT */

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)presentExtent.width;
    viewport.height = (float)presentExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = presentExtent;

    VkPipelineViewportStateCreateInfo viewportInfo;
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.pNext = NULL;
    viewportInfo.flags = 0;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &scissor;

    /* RASTERIZATION */

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

    /* MULTISAMPLING */

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

    /* DEPTH STENCIL */

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

    /* COLOR BLENDING */

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

    /* GRAPHICS PIPELINE */

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
    pipelineCreateInfo.layout = layout;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = 0;

    handleVkResult(
        vkCreateGraphicsPipelines(
            logicalDevice,
            VK_NULL_HANDLE,
            1,
            &pipelineCreateInfo,
            NULL,
            pipeline),
        "creating vox tri graphics pipeline");
}

static void createDebugLinePipeline(
    VkDevice logicalDevice,
    VkPipelineLayout layout,
    VkRenderPass renderPass,
    VkExtent2D presentExtent,
    VkShaderModule vertShaderModule,
    VkShaderModule fragShaderModule,
    VkPipeline* pipeline)
{
    /* SHADER STAGES */
    VkPipelineShaderStageCreateInfo vertShaderStage;
    vertShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStage.pNext = NULL;
    vertShaderStage.flags = 0;
    vertShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStage.module = vertShaderModule;
    vertShaderStage.pName = "main";
    vertShaderStage.pSpecializationInfo = NULL;
    VkPipelineShaderStageCreateInfo fragShaderStage;
    fragShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStage.pNext = NULL;
    fragShaderStage.flags = 0;
    fragShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStage.module = fragShaderModule;
    fragShaderStage.pName = "main";
    fragShaderStage.pSpecializationInfo = NULL;

    /* VERTEX INPUT */
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = NULL;
    vertexInputInfo.flags = 0;
    vertexInputInfo.vertexBindingDescriptionCount = DEBUG_LINE_VERTEX_BINDING_DESCRIPTION_COUNT;
    vertexInputInfo.pVertexBindingDescriptions = DEBUG_LINE_VERTEX_BINDING_DESCRIPTIONS;
    vertexInputInfo.vertexAttributeDescriptionCount = DEBUG_LINE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT;
    vertexInputInfo.pVertexAttributeDescriptions = DEBUG_LINE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS;

    /* INPUT ASSEMBLY */
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.pNext = NULL;
    inputAssemblyInfo.flags = 0;
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    /* VIEWPORT */
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)presentExtent.width;
    viewport.height = (float)presentExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = presentExtent;

    VkPipelineViewportStateCreateInfo viewportInfo;
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.pNext = NULL;
    viewportInfo.flags = 0;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &scissor;

    /* RASTERIZATION */
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

    /* MULTISAMPLING */
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

    /* DEPTH STENCIL */
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

    /* COLOR BLENDING */
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

    /* GRAPHICS PIPELINE */
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
    pipelineCreateInfo.layout = layout;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 1;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = 0;

    handleVkResult(
        vkCreateGraphicsPipelines(
            logicalDevice,
            VK_NULL_HANDLE,
            1,
            &pipelineCreateInfo,
            NULL,
            pipeline),
        "creating debug line graphics pipeline");
}

static void createFramebuffers(
    VkDevice logicalDevice,
    VkRenderPass renderPass,
    VkExtent2D extent,
    uint32_t count,
    VkImageView* swapImageViews,
    VkImageView depthImageView,
    VkFramebuffer* framebuffers)
{
    for (uint32_t i = 0; i < count; i++) {
        VkImageView attachments[] = { swapImageViews[i], depthImageView };

        VkFramebufferCreateInfo framebufferCreateInfo;
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.pNext = NULL;
        framebufferCreateInfo.flags = 0;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = sizeof(attachments) / sizeof(attachments[0]);
        framebufferCreateInfo.pAttachments = attachments;
        framebufferCreateInfo.width = extent.width;
        framebufferCreateInfo.height = extent.height;
        framebufferCreateInfo.layers = 1;

        handleVkResult(
            vkCreateFramebuffer(
                logicalDevice,
                &framebufferCreateInfo,
                NULL,
                &framebuffers[i]),
            "creating framebuffer");
    }
}

static void createRenderCommandBuffers(
    VkDevice logicalDevice,
    VkRenderPass renderPass,
    VkExtent2D presentExtent,
    VkPipeline voxTriPipeline,
    VkPipelineLayout voxTriPipelineLayout,
    VkPipeline debugLinePipeline,
    VkPipelineLayout debugLinePipelineLayout,
    ModelStorage* modelStorage,
    DebugLineStorage* debugLines,
    VkCommandPool pool,
    uint32_t swapLen,
    VkDescriptorSet* cameraDescriptorSets,
    VkFramebuffer* framebuffers,
    VkCommandBuffer* commandBuffers)
{
    /* ALLOCATE COMMAND BUFFERS */
    {
        VkCommandBufferAllocateInfo allocInfo;
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = NULL;
        allocInfo.commandPool = pool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = swapLen;

        handleVkResult(
            vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffers),
            "allocating render command buffers");
    }

    for (int s = 0; s < swapLen; s++) {
        /* BEGIN COMMAND BUFFER */
        VkCommandBufferBeginInfo beginInfo;
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = NULL;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = NULL;
        handleVkResult(
            vkBeginCommandBuffer(commandBuffers[s], &beginInfo),
            "begin recording render command buffers");

        VkClearValue clearValues[2];
        memset(clearValues, 0, sizeof(clearValues));
        clearValues[0].color.float32[0] = 128.0f / 255.0;
        clearValues[0].color.float32[1] = 218.0f / 255.0;
        clearValues[0].color.float32[2] = 251.0f / 255.0;
        clearValues[0].color.float32[3] = 1.0f;

        clearValues[1].depthStencil.depth = 1.0f;

        VkRenderPassBeginInfo renderPassInfo;
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.pNext = NULL;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = framebuffers[s];
        renderPassInfo.renderArea.offset = (VkOffset2D) { 0, 0 };
        renderPassInfo.renderArea.extent = presentExtent;
        renderPassInfo.clearValueCount = sizeof(clearValues) / sizeof(clearValues[0]);
        renderPassInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(
            commandBuffers[s],
            &renderPassInfo,
            VK_SUBPASS_CONTENTS_INLINE);

        /* VOX TRI PIPELINE */
        vkCmdBindPipeline(
            commandBuffers[s],
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            voxTriPipeline);

        vkCmdBindDescriptorSets(
            commandBuffers[s],
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            voxTriPipelineLayout,
            0,
            1,
            &cameraDescriptorSets[s],
            0,
            NULL);

        VkDeviceSize voxTriVertexBufferOffsets[] = { 0 };
        vkCmdBindVertexBuffers(
            commandBuffers[s],
            0,
            1,
            &modelStorage->vertexBuffer,
            voxTriVertexBufferOffsets);

        for (int m = 0; m < modelStorage->idAllocator.maskFilled; m++) {
            if (modelStorage->idAllocator.mask[m] == true) {
                vkCmdBindDescriptorSets(
                    commandBuffers[s],
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    voxTriPipelineLayout,
                    1,
                    1,
                    &modelStorage->uniformDescriptorSets[m],
                    0,
                    NULL);

                vkCmdDraw(
                    commandBuffers[s],
                    modelStorage->vertexCounts[m],
                    1,
                    modelStorage->vertexOffsets[m],
                    0);
            }
        }

        /* DEBUG LINE PIPELINE */
        vkCmdNextSubpass(commandBuffers[s], VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(
            commandBuffers[s],
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            debugLinePipeline);

        vkCmdBindDescriptorSets(
            commandBuffers[s],
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            debugLinePipelineLayout,
            0,
            1,
            &cameraDescriptorSets[s],
            0,
            NULL);

        VkDeviceSize debugLinePipelineVertexBufferOffsets[] = { 0 };

        vkCmdBindVertexBuffers(
            commandBuffers[s],
            0,
            1,
            &debugLines->vertBuffer,
            debugLinePipelineVertexBufferOffsets);

        vkCmdDraw(commandBuffers[s], debugLines->vertCount, 1, 0, 0);

        /* END COMAND BUFFER */
        vkCmdEndRenderPass(commandBuffers[s]);

        handleVkResult(
            vkEndCommandBuffer(commandBuffers[s]),
            "recording render command buffer");
    }
}

void Renderer_init(
    Renderer* renderer,
    VulkanDevice* device,
    VkExtent2D presentExtent)
{
    renderer->presentExtent = presentExtent;

    /* SWAPCHAIN */
    renderer->swapImageFormat = device->physicalProperties.surfaceFormat.format;
    createSwapchain(
        device->logical,
        device->surface,
        device->physicalProperties.surfaceFormat,
        device->physicalProperties.surfaceCapabilities,
        device->physicalProperties.presentMode,
        device->physicalProperties.graphicsFamilyIndex,
        device->physicalProperties.presentFamilyIndex,
        renderer->presentExtent,
        &renderer->swapchain);
    handleVkResult(
        vkGetSwapchainImagesKHR(
            device->logical,
            renderer->swapchain,
            &renderer->swapLen,
            NULL),
        "getting swapchain image count");
    renderer->swapImages
        = (VkImage*)malloc(renderer->swapLen * sizeof(VkImage));
    handleVkResult(
        vkGetSwapchainImagesKHR(
            device->logical,
            renderer->swapchain,
            &renderer->swapLen,
            renderer->swapImages),
        "getting swapchain images");
    renderer->swapImageViews
        = (VkImageView*)malloc(renderer->swapLen * sizeof(VkImageView));
    for (int i = 0; i < renderer->swapLen; i++) {
        VkImageSubresourceRange subresourceRange;
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = 1;
        subresourceRange.baseArrayLayer = 0;
        subresourceRange.layerCount = 1;

        createImageView(
            device->logical,
            renderer->swapImages[i],
            renderer->swapImageFormat,
            VK_IMAGE_VIEW_TYPE_2D,
            subresourceRange,
            &renderer->swapImageViews[i]);
    }

    /* DEPTH IMAGE */
    renderer->depthImageFormat = device->physicalProperties.depthImageFormat;
    createDepthImage(
        device->logical,
        device->physical,
        renderer->depthImageFormat,
        renderer->presentExtent,
        &renderer->depthImage,
        &renderer->depthImageMemory,
        &renderer->depthImageView);

    /* CAMERA DESCRIPTORS */
    createCameraDescriptorSetLayout(
        device->logical,
        &renderer->cameraDescriptorSetLayout);
    createCameraDescriptorPool(
        device->logical,
        renderer->swapLen,
        &renderer->cameraDescriptorPool);
    createBuffer(
        device->logical,
        device->physical,
        renderer->swapLen * sizeof(CameraRenderData),
        0,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &renderer->cameraBuffer,
        &renderer->cameraBufferMemory);
    renderer->cameraDescriptorSets
        = (VkDescriptorSet*)malloc(renderer->swapLen * sizeof(VkDescriptorSet));
    allocateDescriptorSets(
        device->logical,
        renderer->cameraDescriptorSetLayout,
        renderer->cameraDescriptorPool,
        renderer->swapLen,
        renderer->cameraDescriptorSets);
    for (uint32_t i = 0; i < renderer->swapLen; i++) {
        VkDescriptorBufferInfo bufInfo;
        bufInfo.buffer = renderer->cameraBuffer;
        bufInfo.offset = i * sizeof(CameraRenderData);
        bufInfo.range = sizeof(CameraRenderData);

        VkWriteDescriptorSet write;
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.pNext = NULL;
        write.dstSet = renderer->cameraDescriptorSets[i];
        write.dstBinding = 0;
        write.dstArrayElement = 0;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.pImageInfo = NULL;
        write.pBufferInfo = &bufInfo;
        write.pTexelBufferView = NULL;
        vkUpdateDescriptorSets(device->logical, 1, &write, 0, NULL);
    }

    /* SCENE DATA */
    ModelStorage_init(
        &renderer->modelStorage,
        device->logical,
        device->physical);
    DebugLineStorage_init(
        &renderer->debugLineStorage,
        device->logical,
        device->physical,
        2000);

    /* RENDER PASS */
    createRenderPass(
        device->logical,
        renderer->swapImageFormat,
        renderer->depthImageFormat,
        &renderer->renderPass);

    /* SHADER MODULES */
    createShaderModule(
        device->logical,
        "vox_tri.vert.spv",
        &renderer->voxTriVertShader);
    createShaderModule(
        device->logical,
        "vox_tri.frag.spv",
        &renderer->voxTriFragShader);
    createShaderModule(
        device->logical,
        "debug_line.vert.spv",
        &renderer->debugLineVertShader);
    createShaderModule(
        device->logical,
        "debug_line.frag.spv",
        &renderer->debugLineFragShader);

    /* VOX TRI PIPELINE */
    {
        VkDescriptorSetLayout voxTriPipelineDescriptorSetLayouts[] = {
            renderer->cameraDescriptorSetLayout,
            renderer->modelStorage.uniformDescriptorSetLayout
        };

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext = NULL;
        pipelineLayoutCreateInfo.flags = 0;
        pipelineLayoutCreateInfo.setLayoutCount = sizeof(voxTriPipelineDescriptorSetLayouts) / sizeof(voxTriPipelineDescriptorSetLayouts[0]);
        pipelineLayoutCreateInfo.pSetLayouts = voxTriPipelineDescriptorSetLayouts;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = NULL;

        handleVkResult(
            vkCreatePipelineLayout(
                device->logical,
                &pipelineLayoutCreateInfo,
                NULL,
                &renderer->voxTriPipelineLayout),
            "creating pipeline layout");
    }
    createVoxTriPipeline(
        device->logical,
        renderer->voxTriPipelineLayout,
        renderer->renderPass,
        renderer->presentExtent,
        renderer->voxTriVertShader,
        renderer->voxTriFragShader,
        &renderer->voxTriPipeline);

    /* DEBUG LINE PIPELINE */
    {
        VkDescriptorSetLayout debugLinePipelineDescriptorSetLayouts[] = {
            renderer->cameraDescriptorSetLayout,
        };

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext = NULL;
        pipelineLayoutCreateInfo.flags = 0;
        pipelineLayoutCreateInfo.setLayoutCount = sizeof(debugLinePipelineDescriptorSetLayouts) / sizeof(debugLinePipelineDescriptorSetLayouts[0]);
        pipelineLayoutCreateInfo.pSetLayouts = debugLinePipelineDescriptorSetLayouts;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = NULL;

        handleVkResult(
            vkCreatePipelineLayout(
                device->logical,
                &pipelineLayoutCreateInfo,
                NULL,
                &renderer->debugLinePipelineLayout),
            "creating pipeline layout");
    }
    createDebugLinePipeline(
        device->logical,
        renderer->debugLinePipelineLayout,
        renderer->renderPass,
        renderer->presentExtent,
        renderer->debugLineVertShader,
        renderer->debugLineFragShader,
        &renderer->debugLinePipeline);

    /* FRAMEBUFFERS */
    renderer->framebuffers
        = (VkFramebuffer*)malloc(renderer->swapLen * sizeof(VkFramebuffer));
    createFramebuffers(
        device->logical,
        renderer->renderPass,
        renderer->presentExtent,
        renderer->swapLen,
        renderer->swapImageViews,
        renderer->depthImageView,
        renderer->framebuffers);

    /* COMMAND BUFFERS */
    renderer->commandBuffers
        = (VkCommandBuffer*)malloc(renderer->swapLen * sizeof(VkCommandBuffer));
    createRenderCommandBuffers(
        device->logical,
        renderer->renderPass,
        renderer->presentExtent,
        renderer->voxTriPipeline,
        renderer->voxTriPipelineLayout,
        renderer->debugLinePipeline,
        renderer->debugLinePipelineLayout,
        &renderer->modelStorage,
        &renderer->debugLineStorage,
        device->graphicsCommandPool,
        renderer->swapLen,
        renderer->cameraDescriptorSets,
        renderer->framebuffers,
        renderer->commandBuffers);

    /* SYNCHRONIZATION */
    {
        VkSemaphoreCreateInfo semaphoreCreateInfo;
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCreateInfo.pNext = NULL;
        semaphoreCreateInfo.flags = 0;

        VkFenceCreateInfo fenceCreateInfo;
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.pNext = NULL;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            handleVkResult(
                vkCreateSemaphore(
                    device->logical,
                    &semaphoreCreateInfo,
                    NULL,
                    &renderer->imageAvailableSemaphores[i]),
                "creating image available semaphore");
            handleVkResult(
                vkCreateSemaphore(
                    device->logical,
                    &semaphoreCreateInfo,
                    NULL,
                    &renderer->renderFinishedSemaphores[i]),
                "creating render finished semaphore");
            handleVkResult(
                vkCreateFence(
                    device->logical,
                    &fenceCreateInfo,
                    NULL,
                    &renderer->renderFinishedFences[i]),
                "creating render finished fence");
        }
    }

    renderer->swapchainImageFences
        = (VkFence**)malloc(renderer->swapLen * sizeof(VkFence*));
    memset(renderer->swapchainImageFences, 0, renderer->swapLen * sizeof(VkFence*));

    renderer->currentFrame = 0;
}

void Renderer_recreateCommandBuffers(
    Renderer* renderer,
    VulkanDevice* device)
{
    vkDeviceWaitIdle(device->logical);
    createRenderCommandBuffers(
        device->logical,
        renderer->renderPass,
        renderer->presentExtent,
        renderer->voxTriPipeline,
        renderer->voxTriPipelineLayout,
        renderer->debugLinePipeline,
        renderer->debugLinePipelineLayout,
        &renderer->modelStorage,
        &renderer->debugLineStorage,
        device->graphicsCommandPool,
        renderer->swapLen,
        renderer->cameraDescriptorSets,
        renderer->framebuffers,
        renderer->commandBuffers);
}

void Renderer_drawFrame(
    Renderer* renderer,
    VulkanDevice* device,
    CameraRenderData cameraData)
{
    handleVkResult(
        vkWaitForFences(
            device->logical,
            1,
            &renderer->renderFinishedFences[renderer->currentFrame],
            VK_TRUE,
            UINT64_MAX),
        "waiting for render finished fence");

    uint32_t imageIndex;
    handleVkResult(
        vkAcquireNextImageKHR(
            device->logical,
            renderer->swapchain,
            UINT64_MAX,
            renderer->imageAvailableSemaphores[renderer->currentFrame],
            VK_NULL_HANDLE,
            &imageIndex),
        "acquiring next swapchain image");

    if (renderer->swapchainImageFences[imageIndex] != NULL)
        handleVkResult(
            vkWaitForFences(
                device->logical,
                1,
                renderer->swapchainImageFences[imageIndex],
                VK_TRUE,
                UINT64_MAX),
            "waiting for swapchain image render finished fence");

    vkResetFences(
        device->logical,
        1,
        &renderer->renderFinishedFences[renderer->currentFrame]);
    for (int i = 0; i < renderer->swapLen; i++)
        if (renderer->swapchainImageFences[i] == &renderer->renderFinishedFences[renderer->currentFrame]) {
            renderer->swapchainImageFences[i] = NULL;
            break;
        }
    renderer->swapchainImageFences[imageIndex]
        = &renderer->renderFinishedFences[renderer->currentFrame];

    copyDataToBuffer(
        device->logical,
        &cameraData,
        renderer->cameraBufferMemory,
        imageIndex * sizeof(CameraRenderData),
        sizeof(CameraRenderData));

    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &renderer->imageAvailableSemaphores[renderer->currentFrame];
    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &renderer->commandBuffers[imageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderer->renderFinishedSemaphores[renderer->currentFrame];

    handleVkResult(
        vkQueueSubmit(
            device->graphicsQueue,
            1,
            &submitInfo,
            renderer->renderFinishedFences[renderer->currentFrame]),
        "submitting render command buffer");

    VkPresentInfoKHR presentInfo;
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = NULL;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderer->renderFinishedSemaphores[renderer->currentFrame];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &renderer->swapchain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = NULL;

    handleVkResult(
        vkQueuePresentKHR(device->presentQueue, &presentInfo),
        "submitting present call");

    renderer->currentFrame = (renderer->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer_destroy(
    Renderer* renderer,
    VkDevice logicalDevice)
{
    /* CAMERA DESCRIPTORS */
    vkDestroyDescriptorSetLayout(
        logicalDevice,
        renderer->cameraDescriptorSetLayout,
        NULL);
    vkDestroyDescriptorPool(
        logicalDevice,
        renderer->cameraDescriptorPool,
        NULL);
    free(renderer->cameraDescriptorSets);
    vkDestroyBuffer(
        logicalDevice,
        renderer->cameraBuffer,
        NULL);
    vkFreeMemory(
        logicalDevice,
        renderer->cameraBufferMemory,
        NULL);

    /* IMAGES AND FRAMEBUFFERS */
    for (uint32_t i = 0; i < renderer->swapLen; i++) {
        vkDestroyImageView(logicalDevice, renderer->swapImageViews[i], NULL);
        vkDestroyFramebuffer(logicalDevice, renderer->framebuffers[i], NULL);
    }
    vkDestroySwapchainKHR(logicalDevice, renderer->swapchain, NULL);
    vkDestroyImage(logicalDevice, renderer->depthImage, NULL);
    vkFreeMemory(logicalDevice, renderer->depthImageMemory, NULL);
    vkDestroyImageView(logicalDevice, renderer->depthImageView, NULL);
    free(renderer->swapImages);
    free(renderer->swapImageViews);
    free(renderer->framebuffers);

    /* SCENE DATA */
    ModelStorage_destroy(&renderer->modelStorage, logicalDevice);
    DebugLineStorage_destroy(&renderer->debugLineStorage, logicalDevice);

    /* SHADER MODULES */
    vkDestroyShaderModule(logicalDevice, renderer->voxTriVertShader, NULL);
    vkDestroyShaderModule(logicalDevice, renderer->voxTriFragShader, NULL);
    vkDestroyShaderModule(logicalDevice, renderer->debugLineVertShader, NULL);
    vkDestroyShaderModule(logicalDevice, renderer->debugLineFragShader, NULL);

    /* PIPELINES */
    vkDestroyRenderPass(logicalDevice, renderer->renderPass, NULL);
    vkDestroyPipelineLayout(logicalDevice, renderer->voxTriPipelineLayout, NULL);
    vkDestroyPipeline(logicalDevice, renderer->voxTriPipeline, NULL);
    vkDestroyPipelineLayout(logicalDevice, renderer->debugLinePipelineLayout, NULL);
    vkDestroyPipeline(logicalDevice, renderer->debugLinePipeline, NULL);

    /* COMMAND BUFFERS */
    free(renderer->commandBuffers);

    /* SYNCHRONIZATION */
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(logicalDevice, renderer->imageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(logicalDevice, renderer->renderFinishedSemaphores[i], NULL);
        vkDestroyFence(logicalDevice, renderer->renderFinishedFences[i], NULL);
    }
    free(renderer->swapchainImageFences);
}
