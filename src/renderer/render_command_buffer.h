#ifndef RENDER_COMMAND_BUFFER
#define RENDER_COMMAND_BUFFER

#include <vulkan/vulkan.h>

VkCommandBuffer *createRenderCommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    uint32_t count,
    VkRenderPass renderPass,
    VkExtent2D extent,
    VkPipeline graphicsPipeline,
    const VkFramebuffer *framebuffers,
    uint32_t vertexCount,
    VkBuffer vertexBuffer,
    VkCommandBuffer *commandBuffers);

#endif