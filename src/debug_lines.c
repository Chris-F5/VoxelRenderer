#include "./debug_lines.h"

#include <stdio.h>
#include <stdlib.h>

#include "vk_utils/buffer.h"

const VkVertexInputBindingDescription DEBUG_LINE_VERTEX_BINDING_DESCRIPTIONS[] = {
    { 0, sizeof(DebugLineVertex), VK_VERTEX_INPUT_RATE_VERTEX }
};
const size_t DEBUG_LINE_VERTEX_BINDING_DESCRIPTION_COUNT
    = sizeof(DEBUG_LINE_VERTEX_BINDING_DESCRIPTIONS) / sizeof(DEBUG_LINE_VERTEX_BINDING_DESCRIPTIONS[0]);

const VkVertexInputAttributeDescription DEBUG_LINE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[] = {
    { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(DebugLineVertex, pos) },
    { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(DebugLineVertex, color) }
};
const size_t DEBUG_LINE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT
    = sizeof(DEBUG_LINE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS) / sizeof(DEBUG_LINE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[0]);

void DebugLineStorage_init(
    DebugLineStorage* storage,
    VkDevice logicalDevice,
    VkPhysicalDevice physicalDevice,
    uint32_t vertCapacity)
{
    storage->vertCapacity = vertCapacity;
    storage->vertCount = 0;

    createBuffer(
        logicalDevice,
        physicalDevice,
        sizeof(DebugLineVertex) * vertCapacity,
        0,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &storage->vertBuffer,
        &storage->vertBufferMemory);
}

void DebugLineStorage_update(
    DebugLineStorage* storage,
    VkDevice logicalDevice,
    uint32_t vertCount,
    const DebugLineVertex* verts)
{
    if (vertCount > storage->vertCapacity) {
        puts("Failed to update render debug lines. Max vertex count exceeded.");
        exit(EXIT_FAILURE);
    }

    storage->vertCount = vertCount;

    /* TODO: use staging buffer and add sync */

    copyDataToBuffer(
        logicalDevice,
        verts,
        storage->vertBufferMemory,
        0,
        vertCount * sizeof(DebugLineVertex));
}

void DebugLineStorage_destroy(
    DebugLineStorage* storage,
    VkDevice logicalDevice)
{
    vkDestroyBuffer(logicalDevice, storage->vertBuffer, NULL);
    vkFreeMemory(logicalDevice, storage->vertBufferMemory, NULL);
}
