#ifndef CHUNK_LIGHTING
#define CHUNK_LIGHTING

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
} ChunkLighting;

void ChunkLighting_init(
    ChunkLighting* lighting,
    const ChunkGpuStorage* chunkGpuStorage,
    VkDevice logicalDevice,
    VkCommandPool commandPool);

void ChunkLighting_updateChunks(
    ChunkLighting* chunkLighting,
    VkDevice logicalDevice,
    VkQueue queue,
    uint32_t count,
    ChunkRef* chunks,
    vec3 rayDir);

void ChunkLighting_destroy(
    ChunkLighting* lighting,
    VkDevice logicalDevice,
    VkCommandPool commandPool);

#endif
