#ifndef GRAPHICS_PIPELINE
#define GRAPHICS_PIPELINE

#include "vulkan/vulkan.h"

#include "swapchain.h"

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
    VkRenderPass* renderPass);

#endif
