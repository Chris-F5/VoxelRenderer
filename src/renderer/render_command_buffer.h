#ifndef RENDER_COMMAND_BUFFER
#define RENDER_COMMAND_BUFFER

#include <vulkan/vulkan.h>

#include "scene_data/scene_data.h"

void createRenderCommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    uint32_t count,
    VkRenderPass renderPass,
    VkExtent2D extent,
    VkPipeline graphicsPipeline,
    VkPipelineLayout graphicsPipelineLayout,
    const VkFramebuffer* framebuffers,
    const VkDescriptorSet* globalDescriptorSets,
    const SceneData* sceneData,
    VkCommandBuffer* commandBuffers);

#endif
