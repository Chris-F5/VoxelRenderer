#ifndef COMMAND_BUFFER
#define COMMAND_BUFFER

#include "vulkan/vulkan.h"

VkCommandPool createCommandPool(
    VkDevice device,
    VkCommandPoolCreateFlags flags,
    uint32_t queueFamily);

void allocateCommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    size_t count,
    VkCommandBuffer *commandBuffers);

void recordRenderCommandBuffers(
    VkRenderPass renderPass,
    VkExtent2D extent,
    VkPipeline graphicsPipeline,
    uint32_t count,
    VkCommandBuffer *commandBuffers,
    VkFramebuffer *framebuffers,
    uint32_t vertexCount,
    VkBuffer vertexBuffer);

#endif