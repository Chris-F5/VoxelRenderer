#ifndef NORMAL_GEN_H
#define NORMAL_GEN_H

#include <vulkan/vulkan.h>

#include "./chunks.h"

#define NORMAL_X_POS 0
#define NORMAL_X_NEG 1
#define NORMAL_Y_POS 2
#define NORMAL_Y_NEG 3
#define NORMAL_Z_POS 4
#define NORMAL_Z_NEG 5

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
void NormalGen_generateNormals(
    NormalGen* normalGen,
    VkDevice logicalDevice,
    VkQueue queue,
    uint32_t count,
    ChunkRef* chunks);
void NormalGen_destroy(
    NormalGen* normalGen,
    VkDevice logicalDevice,
    VkCommandPool commandPool);

#endif
