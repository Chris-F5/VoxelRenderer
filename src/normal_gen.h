#ifndef NORMAL_GEN_H
#define NORMAL_GEN_H

#include <vulkan/vulkan.h>

#include "./chunks.h"

typedef struct {
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    VkCommandBuffer commandBuffer;
    VkFence fence;
} NormalGen;

void NormalGen_init(
    NormalGen* normalGen,
    const ChunkGpuStorage* chunkGpuStorage,
    VkDevice logicalDevice,
    VkCommandPool commandPool);
void NormalGen_generateNormals();
void NormalGen_destroy();

#endif
