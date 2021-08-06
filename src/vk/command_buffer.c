#include "command_buffer.h"

#include "exceptions.h"

VkCommandPool createCommandPool(
    VkDevice device,
    VkCommandPoolCreateFlags flags,
    uint32_t queueFamily)
{
    VkCommandPoolCreateInfo poolCreateInfo;
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreateInfo.pNext = NULL;
    poolCreateInfo.flags = flags;
    poolCreateInfo.queueFamilyIndex = queueFamily;

    VkCommandPool commandPool;
    handleVkResult(
        vkCreateCommandPool(device, &poolCreateInfo, NULL, &commandPool),
        "creating command pool");

    return commandPool;
}

void allocateCommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    size_t count,
    VkCommandBuffer *commandBuffers)
{
    VkCommandBufferAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    handleVkResult(
        vkAllocateCommandBuffers(device, &allocInfo, commandBuffers),
        "allocating command buffers");
}

void recordRenderCommandBuffers(
    VkRenderPass renderPass,
    VkExtent2D extent,
    VkPipeline graphicsPipeline,
    uint32_t count,
    VkCommandBuffer *commandBuffers,
    VkFramebuffer *framebuffers)
{
    for (int i = 0; i < count; i++)
    {
        VkCommandBufferBeginInfo beginInfo;
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = NULL;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = NULL;
        handleVkResult(
            vkBeginCommandBuffer(commandBuffers[i], &beginInfo),
            "begin recording render command buffers");

        VkClearValue clearColor;
        clearColor.color.float32[0] = 0.0;
        clearColor.color.float32[1] = 0.0;
        clearColor.color.float32[2] = 0.0;
        clearColor.color.float32[3] = 1.0;
        clearColor.depthStencil.depth = 0.0;
        clearColor.depthStencil.stencil = 0.0;

        VkRenderPassBeginInfo renderPassInfo;
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.pNext = NULL;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = framebuffers[i];
        renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
        renderPassInfo.renderArea.extent = extent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        vkCmdEndRenderPass(commandBuffers[i]);

        handleVkResult(
            vkEndCommandBuffer(commandBuffers[i]),
            "recording render command buffer");
    }
}
