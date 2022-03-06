#ifndef CHUNK_LIGHTING
#define CHUNK_LIGHTING

#include <vulkan/vulkan.h>

#include "./chunks.h"

typedef struct {
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;

    VkPipelineLayout directPipelineLayout;
    VkPipeline directPipeline;
    VkCommandBuffer directCommandBuffer;

    VkPipelineLayout diffusePipelineLayout;
    VkPipeline diffusePipeline;
    VkCommandBuffer diffuseCommandBuffer;

    VkFence fence;
} ChunkLighting;

void ChunkLighting_init(
    ChunkLighting* lighting,
    const ChunkGpuStorage* chunkGpuStorage,
    VkDevice logicalDevice,
    VkCommandPool commandPool);

void ChunkLighting_directLightingPass(
    ChunkLighting* chunkLighting,
    VkDevice logicalDevice,
    VkQueue queue,
    uint32_t count,
    ChunkRef* chunks,
    vec3 lightDir);

void ChunkLighting_diffuseLightingPass(
    ChunkLighting* chunkLighting,
    VkDevice logicalDevice,
    VkQueue queue,
    uint32_t count,
    ChunkRef* chunks,
    vec3 lightDir);

void ChunkLighting_destroy(
    ChunkLighting* lighting,
    VkDevice logicalDevice,
    VkCommandPool commandPool);

#endif
