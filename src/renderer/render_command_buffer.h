#ifndef RENDER_COMMAND_BUFFER
#define RENDER_COMMAND_BUFFER

#include <vulkan/vulkan.h>

VkCommandBuffer* createRenderCommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    uint32_t count,
    VkRenderPass renderPass,
    VkExtent2D extent,
    VkPipeline graphicsPipeline,
    VkPipelineLayout graphicsPipelineLayout,
    const VkFramebuffer* framebuffers,
    const VkDescriptorSet* descriptorSets,
    uint32_t modelCount,
    const uint32_t* indexCounts,
    const VkBuffer* vertexBuffers,
    const VkBuffer* indexBuffers,
    VkCommandBuffer* commandBuffers);

#endif
