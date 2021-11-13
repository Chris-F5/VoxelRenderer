#ifndef MODELS_H
#define MODELS_H

#include <stdbool.h>

#include <vulkan/vulkan.h>

#include <cglm/types.h>

#include "./id_allocator.h"

extern const VkVertexInputBindingDescription MODEL_VERTEX_BINDING_DESCRIPTIONS[];
extern const size_t MODEL_VERTEX_BINDING_DESCRIPTION_COUNT;

extern const VkVertexInputAttributeDescription MODEL_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[];
extern const size_t MODEL_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT;

typedef struct {
    vec3 pos;
    vec3 color;
} ModelVertex;

typedef struct {
    mat4 model;
} ModelUniformData;

typedef uint32_t ModelRef;

typedef struct {
    IdAllocator idAllocator;

    /* UNIFORM BUFFERS */
    VkDescriptorSetLayout uniformDescriptorSetLayout;
    VkDescriptorPool uniformDescriptorPool;
    VkDescriptorSet* uniformDescriptorSets;

    VkBuffer uniformsBuffer;
    VkDeviceMemory uniformsBufferMemory;

    /* VERTEX BUFFERS */
    uint32_t totalVertexCapacity;

    uint32_t* vertexOffsets;
    uint32_t* vertexCapacities;
    uint32_t* vertexCounts;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
} ModelStorage;

void ModelStorage_init(
    ModelStorage* storage,
    VkDevice logicalDevice,
    VkPhysicalDevice physicalDevice);

ModelRef ModelStorage_add(
    ModelStorage* storage,
    VkDevice logicalDevice,
    uint32_t vertexCapacity);

void ModelStorage_updateUniformData(
    ModelStorage* storage,
    VkDevice logicalDevice,
    ModelRef model,
    ModelUniformData data);

void ModelStorage_updateVertexData(
    ModelStorage* storage,
    VkDevice logicalDevice,
    ModelRef model,
    uint32_t vertexCount,
    const ModelVertex* vertices);

void ModelStorage_destroy(
    ModelStorage* storage,
    VkDevice logicalDevice);

#endif
