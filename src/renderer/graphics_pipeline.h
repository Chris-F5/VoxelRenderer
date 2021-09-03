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
    PhysicalDeviceProperties physicalDeviceProperties,
    Swapchain swapchain,
    VkShaderModule vertShader,
    VkShaderModule fragShader,
    uint32_t descriptorSetLayoutCount,
    VkDescriptorSetLayout* descriptorSetLayouts);

void cleanupGraphicsPipeline(VkDevice device, GraphicsPipeline pipeline);

#endif
