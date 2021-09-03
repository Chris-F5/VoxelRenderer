#include "render_command_buffer.h"

#include <string.h>

#include "vk_utils/command_buffer.h"
#include "vk_utils/exceptions.h"

VkCommandBuffer* createRenderCommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    uint32_t count,
    VkRenderPass renderPass,
    VkExtent2D extent,
    VkPipeline graphicsPipeline,
    VkPipelineLayout graphicsPipelineLayout,
    const VkFramebuffer* framebuffers,
    const VkDescriptorSet* globalDescriptorSets,
    uint32_t modelCount,
    VkDescriptorSet** meshDescriptorSets,
    const uint32_t* vertexCounts,
    const VkBuffer* vertexBuffers,
    VkCommandBuffer* commandBuffers)
{
    allocateCommandBuffers(device, commandPool, count, commandBuffers);

    for (int i = 0; i < count; i++) {
        VkCommandBufferBeginInfo beginInfo;
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = NULL;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = NULL;
        handleVkResult(
            vkBeginCommandBuffer(commandBuffers[i], &beginInfo),
            "begin recording render command buffers");

        VkClearValue clearValues[2];
        memset(clearValues, 0, sizeof(clearValues));
        clearValues[0].color.float32[0] = 0.0f;
        clearValues[0].color.float32[1] = 0.0f;
        clearValues[0].color.float32[2] = 0.0f;
        clearValues[0].color.float32[3] = 1.0f;

        clearValues[1].depthStencil.depth = 1.0f;

        VkRenderPassBeginInfo renderPassInfo;
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.pNext = NULL;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = framebuffers[i];
        renderPassInfo.renderArea.offset = (VkOffset2D) { 0, 0 };
        renderPassInfo.renderArea.extent = extent;
        renderPassInfo.clearValueCount = sizeof(clearValues) / sizeof(clearValues[0]);
        renderPassInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkDeviceSize vertexBufferOffsets[] = { 0 };

        vkCmdBindDescriptorSets(
            commandBuffers[i],
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            graphicsPipelineLayout,
            0,
            1,
            &globalDescriptorSets[i],
            0,
            NULL);

        for (int m = 0; m < modelCount; m++) {
            vkCmdBindDescriptorSets(
                commandBuffers[i],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                graphicsPipelineLayout,
                1,
                1,
                &meshDescriptorSets[m][i],
                0,
                NULL);

            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vertexBuffers[m], vertexBufferOffsets);

            vkCmdDraw(commandBuffers[i], vertexCounts[m], 1, 0, 0);
        }

        vkCmdEndRenderPass(commandBuffers[i]);

        handleVkResult(
            vkEndCommandBuffer(commandBuffers[i]),
            "recording render command buffer");
    }

    return commandBuffers;
}
