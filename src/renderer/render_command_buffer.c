#include "render_command_buffer.h"

#include <string.h>

#include "vk_utils/command_buffer.h"
#include "vk_utils/exceptions.h"

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
    VkCommandBuffer* commandBuffers)
{
    allocateCommandBuffers(device, commandPool, count, commandBuffers);

    for (int s = 0; s < count; s++) {
        VkCommandBufferBeginInfo beginInfo;
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = NULL;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = NULL;
        handleVkResult(
            vkBeginCommandBuffer(commandBuffers[s], &beginInfo),
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
        renderPassInfo.framebuffer = framebuffers[s];
        renderPassInfo.renderArea.offset = (VkOffset2D) { 0, 0 };
        renderPassInfo.renderArea.extent = extent;
        renderPassInfo.clearValueCount = sizeof(clearValues) / sizeof(clearValues[0]);
        renderPassInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(commandBuffers[s], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers[s], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkDeviceSize vertexBufferOffsets[] = { 0 };

        vkCmdBindDescriptorSets(
            commandBuffers[s],
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            graphicsPipelineLayout,
            0,
            1,
            &globalDescriptorSets[s],
            0,
            NULL);

        for (int b = 0; b < sceneData->allocatedBlocks; b++) {
            vkCmdBindDescriptorSets(
                commandBuffers[s],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                graphicsPipelineLayout,
                1,
                1,
                &sceneData->blockDescriptorSets[b],
                0,
                NULL);

            vkCmdBindVertexBuffers(commandBuffers[s], 0, 1, &sceneData->vertexBuffers[b], vertexBufferOffsets);

            vkCmdDraw(commandBuffers[s], sceneData->vertexBuffersLength[b], 1, 0, 0);
        }

        vkCmdEndRenderPass(commandBuffers[s]);

        handleVkResult(
            vkEndCommandBuffer(commandBuffers[s]),
            "recording render command buffer");
    }
}
