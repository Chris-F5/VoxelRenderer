#include "./chunk_lighting.h"

#include "./vk_utils/descriptor_set.h"
#include "./vk_utils/exceptions.h"
#include "./vk_utils/shader_module.h"

typedef struct {
    uint32_t thisChunk;
    uint32_t chunkScale;
    uint32_t chunkVoxCount;
} LightingPushConstants;

static void recordLightingCommandBuffers(
    VkPipeline pipeline,
    VkPipelineLayout pipelineLayout,
    VkDescriptorSet descriptorSet,
    unsigned int count,
    VkCommandBuffer* commandBuffers,
    LightingPushConstants* pushConstants)
{
    VkCommandBufferBeginInfo beginInfo;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = NULL;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    for (unsigned int i = 0; i < count; i++) {
        handleVkResult(
            vkBeginCommandBuffer(commandBuffers[i], &beginInfo),
            "begin recording render command buffers");

        vkCmdBindPipeline(
            commandBuffers[i],
            VK_PIPELINE_BIND_POINT_COMPUTE,
            pipeline);

        vkCmdBindDescriptorSets(
            commandBuffers[i],
            VK_PIPELINE_BIND_POINT_COMPUTE,
            pipelineLayout,
            0,
            1,
            &descriptorSet,
            0, NULL);

        vkCmdPushConstants(
            commandBuffers[i],
            pipelineLayout,
            VK_SHADER_STAGE_COMPUTE_BIT,
            0,
            sizeof(pushConstants[i]),
            &pushConstants[i]);

        vkCmdDispatch(
            commandBuffers[i],
            CHUNK_SCALE,
            CHUNK_SCALE,
            CHUNK_SCALE);

        handleVkResult(
            vkEndCommandBuffer(commandBuffers[i]),
            "ending chunk lighting command buffer");
    }
}

void ChunkLighting_init(
    ChunkLighting* lighting,
    const ChunkGpuStorage* chunkGpuStorage,
    VkDevice logicalDevice,
    VkCommandPool commandPool)
{
    /* DESCRIPTOR POOL */
    {
        VkDescriptorPoolSize poolSize;
        poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSize.descriptorCount = 3;

        VkDescriptorPoolCreateInfo poolCreateInfo;
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolCreateInfo.pNext = NULL;
        poolCreateInfo.flags = 0;
        poolCreateInfo.maxSets = 1;
        poolCreateInfo.poolSizeCount = 1;
        poolCreateInfo.pPoolSizes = &poolSize;

        handleVkResult(
            vkCreateDescriptorPool(
                logicalDevice,
                &poolCreateInfo,
                NULL,
                &lighting->descriptorPool),
            "creating chunk lighting descriptor pool");
    }

    /* CREATE DESCRIPTOR SET */
    {
        VkDescriptorSetLayoutBinding chunkBrightnessBinding;
        chunkBrightnessBinding.binding = 0;
        chunkBrightnessBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        chunkBrightnessBinding.descriptorCount = 1;
        chunkBrightnessBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        chunkBrightnessBinding.pImmutableSamplers = NULL;

        VkDescriptorSetLayoutBinding chunkVoxMaskBinding;
        chunkVoxMaskBinding.binding = 1;
        chunkVoxMaskBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        chunkVoxMaskBinding.descriptorCount = 1;
        chunkVoxMaskBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        chunkVoxMaskBinding.pImmutableSamplers = NULL;

        VkDescriptorSetLayoutBinding chunkNeighbourBinding;
        chunkNeighbourBinding.binding = 2;
        chunkNeighbourBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        chunkNeighbourBinding.descriptorCount = 1;
        chunkNeighbourBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        chunkNeighbourBinding.pImmutableSamplers = NULL;

        VkDescriptorSetLayoutBinding bindings[] = {
            chunkBrightnessBinding,
            chunkVoxMaskBinding,
            chunkNeighbourBinding
        };

        VkDescriptorSetLayoutCreateInfo layoutCreateInfo;
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.pNext = NULL;
        layoutCreateInfo.flags = 0;
        layoutCreateInfo.bindingCount = sizeof(bindings) / sizeof(bindings[0]);
        layoutCreateInfo.pBindings = bindings;

        handleVkResult(
            vkCreateDescriptorSetLayout(
                logicalDevice,
                &layoutCreateInfo,
                NULL,
                &lighting->descriptorSetLayout),
            "creating chunk lighting descriptor set layout");

        allocateDescriptorSets(
            logicalDevice,
            lighting->descriptorSetLayout,
            lighting->descriptorPool,
            1,
            &lighting->descriptorSet);
    }

    /* WRITE DESCRIPTOR SET */
    {
        VkDescriptorBufferInfo brightnessBufferInfo;
        brightnessBufferInfo.buffer = chunkGpuStorage->brightness;
        brightnessBufferInfo.offset = 0;
        brightnessBufferInfo.range = VK_WHOLE_SIZE;

        VkWriteDescriptorSet brightnessWrite;
        brightnessWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        brightnessWrite.pNext = NULL;
        brightnessWrite.dstSet = lighting->descriptorSet;
        brightnessWrite.dstBinding = 0;
        brightnessWrite.dstArrayElement = 0;
        brightnessWrite.descriptorCount = 1;
        brightnessWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        brightnessWrite.pImageInfo = NULL;
        brightnessWrite.pBufferInfo = &brightnessBufferInfo;
        brightnessWrite.pTexelBufferView = NULL;

        VkDescriptorBufferInfo voxMaskBufferInfo;
        voxMaskBufferInfo.buffer = chunkGpuStorage->voxBitMask;
        voxMaskBufferInfo.offset = 0;
        voxMaskBufferInfo.range = VK_WHOLE_SIZE;

        VkWriteDescriptorSet voxMaskWrite;
        voxMaskWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        voxMaskWrite.pNext = NULL;
        voxMaskWrite.dstSet = lighting->descriptorSet;
        voxMaskWrite.dstBinding = 1;
        voxMaskWrite.dstArrayElement = 0;
        voxMaskWrite.descriptorCount = 1;
        voxMaskWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        voxMaskWrite.pImageInfo = NULL;
        voxMaskWrite.pBufferInfo = &voxMaskBufferInfo;
        voxMaskWrite.pTexelBufferView = NULL;

        VkDescriptorBufferInfo neighbourBufferInfo;
        neighbourBufferInfo.buffer = chunkGpuStorage->neighbours;
        neighbourBufferInfo.offset = 0;
        neighbourBufferInfo.range = VK_WHOLE_SIZE;

        VkWriteDescriptorSet neighbourWrite;
        neighbourWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        neighbourWrite.pNext = NULL;
        neighbourWrite.dstSet = lighting->descriptorSet;
        neighbourWrite.dstBinding = 2;
        neighbourWrite.dstArrayElement = 0;
        neighbourWrite.descriptorCount = 1;
        neighbourWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        neighbourWrite.pImageInfo = NULL;
        neighbourWrite.pBufferInfo = &neighbourBufferInfo;
        neighbourWrite.pTexelBufferView = NULL;

        VkWriteDescriptorSet writes[] = {
            brightnessWrite,
            voxMaskWrite,
            neighbourWrite
        };

        vkUpdateDescriptorSets(
            logicalDevice,
            sizeof(writes) / sizeof(writes[0]),
            writes,
            0, NULL);
    }

    /* PIPELINE LAYOUT */
    {
        VkDescriptorSetLayout descriptorSetsLayouts[]
            = { lighting->descriptorSetLayout };

        VkPushConstantRange pushConstant;
        pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        pushConstant.offset = 0;
        pushConstant.size = sizeof(LightingPushConstants);

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext = NULL;
        pipelineLayoutCreateInfo.flags = 0;
        pipelineLayoutCreateInfo.setLayoutCount
            = sizeof(descriptorSetsLayouts) / sizeof(descriptorSetsLayouts[0]);
        pipelineLayoutCreateInfo.pSetLayouts = &lighting->descriptorSetLayout;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;

        handleVkResult(
            vkCreatePipelineLayout(
                logicalDevice,
                &pipelineLayoutCreateInfo,
                NULL,
                &lighting->pipelineLayout),
            "creating chunk lighting pipeline layout");
    }

    /* PIPELINE */
    {
        VkShaderModule shaderModule;
        createShaderModule(
            logicalDevice,
            "chunk_lighting.comp.spv",
            &shaderModule);

        VkPipelineShaderStageCreateInfo shaderStage;
        shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStage.pNext = NULL;
        shaderStage.flags = 0;
        shaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        shaderStage.module = shaderModule;
        shaderStage.pName = "main";
        shaderStage.pSpecializationInfo = NULL;

        VkComputePipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.pNext = NULL;
        pipelineCreateInfo.flags = 0;
        pipelineCreateInfo.stage = shaderStage;
        pipelineCreateInfo.layout = lighting->pipelineLayout;
        pipelineCreateInfo.basePipelineHandle = NULL;
        pipelineCreateInfo.basePipelineIndex = 0;

        handleVkResult(
            vkCreateComputePipelines(
                logicalDevice,
                VK_NULL_HANDLE,
                1,
                &pipelineCreateInfo,
                NULL,
                &lighting->pipeline),
            "creating chunk lighting pipeline");
    }

    /* ALLOCATE COMMAND BUFFER */
    {
        VkCommandBufferAllocateInfo allocInfo;
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = NULL;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        handleVkResult(
            vkAllocateCommandBuffers(
                logicalDevice,
                &allocInfo,
                &lighting->commandBuffer),
            "allocating chunk lighting command buffers");
    }

    /* FENCE */
    {
        VkFenceCreateInfo fenceCreateInfo;
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.pNext = NULL;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        handleVkResult(
            vkCreateFence(
                logicalDevice,
                &fenceCreateInfo,
                NULL,
                &lighting->fence),
            "creating chunk lighting fence");
    }
}

void ChunkLighting_updateChunks(
    ChunkLighting* chunkLighting,
    VkDevice logicalDevice,
    VkQueue queue,
    uint32_t count,
    ChunkRef* chunks)
{
    for (uint32_t i = 0; i < count; i++) {
        handleVkResult(
            vkWaitForFences(
                logicalDevice,
                1,
                &chunkLighting->fence,
                VK_TRUE,
                UINT64_MAX),
            "waiting for chunk lighting fence");
        vkResetCommandBuffer(chunkLighting->commandBuffer, 0);

        {
            LightingPushConstants pushConstant;
            pushConstant.chunkScale = CHUNK_SCALE;
            pushConstant.chunkVoxCount = CHUNK_VOX_COUNT;
            pushConstant.thisChunk = chunks[i];
            recordLightingCommandBuffers(
                chunkLighting->pipeline,
                chunkLighting->pipelineLayout,
                chunkLighting->descriptorSet,
                1,
                &chunkLighting->commandBuffer,
                &pushConstant);
        }

        handleVkResult(
            vkResetFences(logicalDevice, 1, &chunkLighting->fence),
            "resetting chunk lighting fence");
        VkSubmitInfo submitInfo;
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = NULL;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitDstStageMask = 0;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &chunkLighting->commandBuffer;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = NULL;

        handleVkResult(
            vkQueueSubmit(
                queue,
                1,
                &submitInfo,
                chunkLighting->fence),
            "submitting chunk lighting update command buffer");
    }
    handleVkResult(
        vkWaitForFences(
            logicalDevice,
            1,
            &chunkLighting->fence,
            VK_TRUE,
            UINT64_MAX),
        "waiting for chunk lighting fence");
}

void ChunkLighting_destroy(
    ChunkLighting* lighting,
    VkDevice logicalDevice,
    VkCommandPool commandPool)
{
    vkDestroyDescriptorPool(logicalDevice, lighting->descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(logicalDevice, lighting->descriptorSetLayout, NULL);
    vkDestroyPipelineLayout(logicalDevice, lighting->pipelineLayout, NULL);
    vkDestroyPipeline(logicalDevice, lighting->pipeline, NULL);
    vkFreeCommandBuffers(logicalDevice, commandPool, 1, &lighting->commandBuffer);
    vkDestroyFence(logicalDevice, lighting->fence, NULL);
}
