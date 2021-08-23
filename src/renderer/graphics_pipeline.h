#ifndef GRAPHICS_PIPELINE
#define GRAPHICS_PIPELINE

#include "vulkan/vulkan.h"

#include "swapchain.h"

typedef struct
{
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    VkRenderPass renderPass;
} GraphicsPipeline;

GraphicsPipeline createGraphicsPipeline(
    VkDevice device,
    Swapchain swapchain,
    VkShaderModule vertShader,
    VkShaderModule fragShader,
    VkDescriptorSetLayout descriptorSetLayout);

void cleanupGraphicsPipeline(VkDevice device, GraphicsPipeline pipeline);

#endif
