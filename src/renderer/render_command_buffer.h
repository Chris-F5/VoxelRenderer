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
    uint32_t indexCount,
    VkBuffer vertexBuffer,
    VkBuffer indexBuffer,
    VkCommandBuffer* commandBuffers);

#endif
