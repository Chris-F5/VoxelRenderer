#include "./normal_gen.h"

#include "./vk_utils/exceptions.h"
#include "./vk_utils/descriptor_set.h"
#include "./vk_utils/shader_module.h"

typedef struct {
    uint32_t thisChunk;
    uint32_t chunkScale;
    uint32_t chunkVoxCount;
} NormalGenPushConstants;

static void recordNormalGenCommandBuffers(
    VkPipeline pipeline,
    VkPipelineLayout pipelineLayout,
    VkDescriptorSet descriptorSet,
    unsigned int count,
    VkCommandBuffer* commandBuffers,
    NormalGenPushConstants* pushConstants)
{
    VkCommandBufferBeginInfo beginInfo;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = NULL;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    for (unsigned int i = 0; i < count; i++) {
        handleVkResult(
            vkBeginCommandBuffer(commandBuffers[i], &beginInfo),
            "begin recording normal gen command buffers");

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
            "ending normal gen command buffer");
    }
}

void NormalGen_init(
    NormalGen* normalGen,
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
                &normalGen->descriptorPool),
            "creating normal gen descriptor pool");
    }

    /* CREATE DESCRIPTOR SET */
    {
        VkDescriptorSetLayoutBinding chunkVoxMaskBinding;
        chunkVoxMaskBinding.binding = 0;
        chunkVoxMaskBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        chunkVoxMaskBinding.descriptorCount = 1;
        chunkVoxMaskBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        chunkVoxMaskBinding.pImmutableSamplers = NULL;

        VkDescriptorSetLayoutBinding chunkNormalBinding;
        chunkNormalBinding.binding = 1;
        chunkNormalBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        chunkNormalBinding.descriptorCount = 1;
        chunkNormalBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        chunkNormalBinding.pImmutableSamplers = NULL;

        VkDescriptorSetLayoutBinding chunkNeighbourBinding;
        chunkNeighbourBinding.binding = 2;
        chunkNeighbourBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        chunkNeighbourBinding.descriptorCount = 1;
        chunkNeighbourBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        chunkNeighbourBinding.pImmutableSamplers = NULL;

        VkDescriptorSetLayoutBinding bindings[] = {
            chunkVoxMaskBinding,
            chunkNormalBinding,
            chunkNeighbourBinding,
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
                &normalGen->descriptorSetLayout),
            "creating normal gen descriptor set layout");

        allocateDescriptorSets(
            logicalDevice,
            normalGen->descriptorSetLayout,
            normalGen->descriptorPool,
            1,
            &normalGen->descriptorSet);
    }

    /* WRITE DESCRIPTOR SET */
    {
        VkDescriptorBufferInfo voxMaskBufferInfo;
        voxMaskBufferInfo.buffer = chunkGpuStorage->voxBitMask;
        voxMaskBufferInfo.offset = 0;
        voxMaskBufferInfo.range = VK_WHOLE_SIZE;

        VkWriteDescriptorSet voxMaskWrite;
        voxMaskWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        voxMaskWrite.pNext = NULL;
        voxMaskWrite.dstSet = normalGen->descriptorSet;
        voxMaskWrite.dstBinding = 0;
        voxMaskWrite.dstArrayElement = 0;
        voxMaskWrite.descriptorCount = 1;
        voxMaskWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        voxMaskWrite.pImageInfo = NULL;
        voxMaskWrite.pBufferInfo = &voxMaskBufferInfo;
        voxMaskWrite.pTexelBufferView = NULL;

        VkDescriptorBufferInfo normalBufferInfo;
        normalBufferInfo.buffer = chunkGpuStorage->normals;
        normalBufferInfo.offset = 0;
        normalBufferInfo.range = VK_WHOLE_SIZE;

        VkWriteDescriptorSet normalWrite;
        normalWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        normalWrite.pNext = NULL;
        normalWrite.dstSet = normalGen->descriptorSet;
        normalWrite.dstBinding = 1;
        normalWrite.dstArrayElement = 0;
        normalWrite.descriptorCount = 1;
        normalWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        normalWrite.pImageInfo = NULL;
        normalWrite.pBufferInfo = &normalBufferInfo;
        normalWrite.pTexelBufferView = NULL;

        VkDescriptorBufferInfo neighbourBufferInfo;
        neighbourBufferInfo.buffer = chunkGpuStorage->neighbours;
        neighbourBufferInfo.offset = 0;
        neighbourBufferInfo.range = VK_WHOLE_SIZE;

        VkWriteDescriptorSet neighbourWrite;
        neighbourWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        neighbourWrite.pNext = NULL;
        neighbourWrite.dstSet = normalGen->descriptorSet;
        neighbourWrite.dstBinding = 2;
        neighbourWrite.dstArrayElement = 0;
        neighbourWrite.descriptorCount = 1;
        neighbourWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        neighbourWrite.pImageInfo = NULL;
        neighbourWrite.pBufferInfo = &neighbourBufferInfo;
        neighbourWrite.pTexelBufferView = NULL;

        VkWriteDescriptorSet writes[] = {
            voxMaskWrite,
            normalWrite,
            neighbourWrite,
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
            = { normalGen->descriptorSetLayout };

        VkPushConstantRange pushConstant;
        pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        pushConstant.offset = 0;
        pushConstant.size = sizeof(NormalGenPushConstants);

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext = NULL;
        pipelineLayoutCreateInfo.flags = 0;
        pipelineLayoutCreateInfo.setLayoutCount
            = sizeof(descriptorSetsLayouts) / sizeof(descriptorSetsLayouts[0]);
        pipelineLayoutCreateInfo.pSetLayouts = &normalGen->descriptorSetLayout;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;

        handleVkResult(
            vkCreatePipelineLayout(
                logicalDevice,
                &pipelineLayoutCreateInfo,
                NULL,
                &normalGen->pipelineLayout),
            "creating normal gen pipeline layout");
    }

    /* PIPELINE */
    {
        VkShaderModule shaderModule;
        createShaderModule(
            logicalDevice,
            "normal_gen.comp.spv",
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
        pipelineCreateInfo.layout = normalGen->pipelineLayout;
        pipelineCreateInfo.basePipelineHandle = NULL;
        pipelineCreateInfo.basePipelineIndex = 0;

        handleVkResult(
            vkCreateComputePipelines(
                logicalDevice,
                VK_NULL_HANDLE,
                1,
                &pipelineCreateInfo,
                NULL,
                &normalGen->pipeline),
            "creating normal gen pipeline");
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
                &normalGen->commandBuffer),
            "allocating normal gen command buffers");
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
                &normalGen->fence),
            "creating normal gen fence");
    }
}

void NormalGen_generateNormals(
    NormalGen* normalGen,
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
                &normalGen->fence,
                VK_TRUE,
                UINT64_MAX),
            "waiting for normal gen fence");
        vkResetCommandBuffer(normalGen->commandBuffer, 0);

        {
            NormalGenPushConstants pushConstant;
            pushConstant.chunkScale = CHUNK_SCALE;
            pushConstant.chunkVoxCount = CHUNK_VOX_COUNT;
            pushConstant.thisChunk = chunks[i];
            recordNormalGenCommandBuffers(
                normalGen->pipeline,
                normalGen->pipelineLayout,
                normalGen->descriptorSet,
                1,
                &normalGen->commandBuffer,
                &pushConstant);
        }

        handleVkResult(
            vkResetFences(logicalDevice, 1, &normalGen->fence),
            "resetting normal gen fence");
        VkSubmitInfo submitInfo;
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = NULL;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitDstStageMask = 0;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &normalGen->commandBuffer;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = NULL;

        handleVkResult(
            vkQueueSubmit(
                queue,
                1,
                &submitInfo,
                normalGen->fence),
            "submitting normal gen command buffer");
    }
    handleVkResult(
        vkWaitForFences(
            logicalDevice,
            1,
            &normalGen->fence,
            VK_TRUE,
            UINT64_MAX),
        "waiting for normal gen fence");
}

void NormalGen_destroy(
    NormalGen* normalGen,
    VkDevice logicalDevice,
    VkCommandPool commandPool)
{
    vkDestroyDescriptorPool(logicalDevice, normalGen->descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(logicalDevice, normalGen->descriptorSetLayout, NULL);
    vkDestroyPipelineLayout(logicalDevice, normalGen->pipelineLayout, NULL);
    vkDestroyPipeline(logicalDevice, normalGen->pipeline, NULL);
    vkFreeCommandBuffers(logicalDevice, commandPool, 1, &normalGen->commandBuffer);
    vkDestroyFence(logicalDevice, normalGen->fence, NULL);
}
