#include "./models.h"

#include <stdio.h>
#include <stdlib.h>

#include "vk_utils/buffer.h"
#include "vk_utils/descriptor_set.h"
#include "vk_utils/exceptions.h"

static const uint32_t MODEL_CAPACITY = 100;
static const uint32_t TOTAL_VERTEX_CAPACITY = 1000000;

const VkVertexInputBindingDescription MODEL_VERTEX_BINDING_DESCRIPTIONS[] = {
    { 0, sizeof(ModelVertex), VK_VERTEX_INPUT_RATE_VERTEX }
};
const size_t MODEL_VERTEX_BINDING_DESCRIPTION_COUNT
    = sizeof(MODEL_VERTEX_BINDING_DESCRIPTIONS) / sizeof(MODEL_VERTEX_BINDING_DESCRIPTIONS[0]);

const VkVertexInputAttributeDescription MODEL_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[] = {
    { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ModelVertex, pos) },
    { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ModelVertex, color) },
};
const size_t MODEL_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT
    = sizeof(MODEL_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS) / sizeof(MODEL_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[0]);

static VkDescriptorSetLayout createModelUniformDescriptorSetLayout(VkDevice device)
{
    VkDescriptorSetLayoutBinding uboBinding;
    uboBinding.binding = 0;
    uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboBinding.descriptorCount = 1;
    uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboBinding.pImmutableSamplers = NULL;

    VkDescriptorSetLayoutCreateInfo layoutCreateInfo;
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreateInfo.pNext = NULL;
    layoutCreateInfo.flags = 0;
    layoutCreateInfo.bindingCount = 1;
    layoutCreateInfo.pBindings = &uboBinding;

    VkDescriptorSetLayout layout;
    handleVkResult(
        vkCreateDescriptorSetLayout(device, &layoutCreateInfo, NULL, &layout),
        "creating model uniform descriptor set layout");

    return layout;
}

static VkDescriptorPool createModelUniformDescriptorPool(
    VkDevice device,
    uint32_t maxDescriptorSetCount)
{
    VkDescriptorPoolSize poolSize;
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = maxDescriptorSetCount;

    VkDescriptorPoolCreateInfo poolCreateInfo;
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.pNext = NULL;
    poolCreateInfo.flags = 0;
    poolCreateInfo.maxSets = maxDescriptorSetCount;
    poolCreateInfo.poolSizeCount = 1;
    poolCreateInfo.pPoolSizes = &poolSize;

    VkDescriptorPool pool;
    handleVkResult(
        vkCreateDescriptorPool(device, &poolCreateInfo, NULL, &pool),
        "creating model uniform descriptor pool");

    return pool;
}

static void bindModelUniformDescriptorSets(
    VkDevice device,
    uint32_t setCount,
    const VkDescriptorSet* descriptorSets,
    const VkDescriptorBufferInfo* blockRenderInfoUniformBufferInfos)
{
    for (int i = 0; i < setCount; i++) {
        VkWriteDescriptorSet uniformBufferDescriptorWrite;
        uniformBufferDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uniformBufferDescriptorWrite.pNext = NULL;
        uniformBufferDescriptorWrite.dstSet = descriptorSets[i];
        uniformBufferDescriptorWrite.dstBinding = 0;
        uniformBufferDescriptorWrite.dstArrayElement = 0;
        uniformBufferDescriptorWrite.descriptorCount = 1;
        uniformBufferDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferDescriptorWrite.pImageInfo = NULL;
        uniformBufferDescriptorWrite.pBufferInfo = &blockRenderInfoUniformBufferInfos[i];
        uniformBufferDescriptorWrite.pTexelBufferView = NULL;

        vkUpdateDescriptorSets(device, 1, &uniformBufferDescriptorWrite, 0, NULL);
    }
}

void ModelStorage_init(
    ModelStorage* storage,
    VkDevice logicalDevice,
    VkPhysicalDevice physicalDevice)
{
    IdAllocator_init(&storage->idAllocator, MODEL_CAPACITY);

    /* UNIFORM BUFFERS */
    storage->uniformDescriptorSetLayout
        = createModelUniformDescriptorSetLayout(logicalDevice);
    storage->uniformDescriptorPool
        = createModelUniformDescriptorPool(logicalDevice, MODEL_CAPACITY);
    storage->uniformDescriptorSets
        = (VkDescriptorSet*)malloc(MODEL_CAPACITY * sizeof(VkDescriptorSet));
    for (int i = 0; i < MODEL_CAPACITY; i++) {
        storage->uniformDescriptorSets[i] = VK_NULL_HANDLE;
    }

    createBuffer(
        logicalDevice,
        physicalDevice,
        MODEL_CAPACITY * sizeof(ModelUniformData),
        0,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &storage->uniformsBuffer,
        &storage->uniformsBufferMemory);

    /* VERTEX BUFFERS */
    storage->totalVertexCapacity = TOTAL_VERTEX_CAPACITY;

    storage->vertexOffsets
        = (uint32_t*)malloc(MODEL_CAPACITY * sizeof(uint32_t));
    storage->vertexCapacities
        = (uint32_t*)malloc(MODEL_CAPACITY * sizeof(uint32_t));
    storage->vertexCounts
        = (uint32_t*)malloc(MODEL_CAPACITY * sizeof(uint32_t));

    createBuffer(
        logicalDevice,
        physicalDevice,
        storage->totalVertexCapacity * sizeof(ModelVertex),
        0,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &storage->vertexBuffer,
        &storage->vertexBufferMemory);
}

ModelRef ModelStorage_add(
    ModelStorage* storage,
    VkDevice logicalDevice,
    uint32_t vertexCapacity)
{
    uint32_t modelIndex = IdAllocator_add(&storage->idAllocator);

    /* UNIFORM BUFFER */
    allocateDescriptorSets(
        logicalDevice,
        storage->uniformDescriptorSetLayout,
        storage->uniformDescriptorPool,
        1,
        &storage->uniformDescriptorSets[modelIndex]);

    VkDescriptorBufferInfo uniformBufferInfo;
    uniformBufferInfo.offset = modelIndex * sizeof(ModelUniformData);
    uniformBufferInfo.range = sizeof(ModelUniformData);
    uniformBufferInfo.buffer = storage->uniformsBuffer;

    bindModelUniformDescriptorSets(
        logicalDevice,
        1,
        &storage->uniformDescriptorSets[modelIndex],
        &uniformBufferInfo);

    /* VERTEX BUFFER */

    /* TODO: Fill gaps left by removed models instead of just appending to end. */

    uint32_t offset = 0;
    for (int i = 0; i < storage->idAllocator.maskFilled; i++)
        if (i != modelIndex
            && storage->idAllocator.mask[i] == true
            && storage->vertexOffsets[i] >= offset)
            offset = storage->vertexOffsets[i] + storage->vertexCapacities[i];

    if (offset + vertexCapacity > storage->totalVertexCapacity) {
        puts("Failed to add render model. Vertex buffer full.");
        exit(EXIT_FAILURE);
    }

    storage->vertexOffsets[modelIndex] = offset;
    storage->vertexCapacities[modelIndex] = vertexCapacity;
    storage->vertexCounts[modelIndex] = 0;

    return modelIndex;
}

void ModelStorage_updateUniformData(
    ModelStorage* storage,
    VkDevice logicalDevice,
    ModelRef model,
    ModelUniformData data)
{
    /* TODO: use staging buffer and add sync */

    copyDataToBuffer(
        logicalDevice,
        &data,
        storage->uniformsBufferMemory,
        model * sizeof(ModelUniformData),
        sizeof(ModelUniformData));
}

void ModelStorage_updateVertexData(
    ModelStorage* storage,
    VkDevice logicalDevice,
    ModelRef model,
    uint32_t vertexCount,
    const ModelVertex* vertices)
{
    /* TODO: use staging buffer and add sync */

    if (vertexCount > storage->vertexCapacities[model]) {
        /* TODO: reallocate model vertex buffer */
        puts("Failed to update render model vertices. Exceeded this models vertex capacity.");
    }
    storage->vertexCounts[model] = vertexCount;

    copyDataToBuffer(
        logicalDevice,
        vertices,
        storage->vertexBufferMemory,
        storage->vertexOffsets[model] * sizeof(ModelVertex),
        vertexCount * sizeof(ModelVertex));
}

void ModelStorage_destroy(
    ModelStorage* storage,
    VkDevice logicalDevice)
{
    vkDestroyDescriptorSetLayout(logicalDevice, storage->uniformDescriptorSetLayout, NULL);
    vkDestroyDescriptorPool(logicalDevice, storage->uniformDescriptorPool, NULL);
    free(storage->uniformDescriptorSets);

    vkDestroyBuffer(logicalDevice, storage->uniformsBuffer, NULL);
    vkFreeMemory(logicalDevice, storage->uniformsBufferMemory, NULL);

    free(storage->vertexOffsets);
    free(storage->vertexCapacities);
    free(storage->vertexCounts);

    vkDestroyBuffer(logicalDevice, storage->vertexBuffer, NULL);
    vkFreeMemory(logicalDevice, storage->vertexBufferMemory, NULL);
}
