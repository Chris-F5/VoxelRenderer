#include "render_command_buffer.h"

#include "vk_utils/exceptions.h"
#include "vk_utils/command_buffer.h"

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
    VkCommandBuffer *commandBuffers)
{
    allocateCommandBuffers(device, commandPool, count, commandBuffers);

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
        clearColor.color.int32[0] = 0;
        clearColor.color.int32[1] = 0;
        clearColor.color.int32[2] = 0;
        clearColor.color.int32[3] = 0;
        clearColor.color.uint32[0] = 0;
        clearColor.color.uint32[1] = 0;
        clearColor.color.uint32[2] = 0;
        clearColor.color.uint32[3] = 0;
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

        VkDeviceSize vertexBufferOffsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vertexBuffer, vertexBufferOffsets);

        vkCmdDraw(commandBuffers[i], vertexCount, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffers[i]);

        handleVkResult(
            vkEndCommandBuffer(commandBuffers[i]),
            "recording render command buffer");
    }

    return commandBuffers;
}
