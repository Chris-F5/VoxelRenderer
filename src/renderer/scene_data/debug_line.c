#include "debug_line.h"

#include <string.h>
#include <stdlib.h>

#include "../vk_utils/buffer.h"

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

DebugLineData createEmptyDebugLineData(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t initialCapacity)
{
    DebugLineData debugLineData;
    debugLineData.capacity = initialCapacity;
    debugLineData.vertCount = 0;
    debugLineData.verts = (DebugLineVertex*)
        malloc(initialCapacity * sizeof(DebugLineVertex));
    createBuffer(
        device,
        physicalDevice,
        initialCapacity * sizeof(DebugLineVertex),
        0,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        &debugLineData.vertBuffer,
        &debugLineData.vertBufferMemory);

    return debugLineData;
}

void updateDebugLineData(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t newVertsLength,
    const DebugLineVertex* newVerts,
    DebugLineData* debugLineData)
{
    debugLineData->vertCount = newVertsLength;
    if (newVertsLength > debugLineData->capacity) {
        debugLineData->capacity = newVertsLength;
        debugLineData->verts = (DebugLineVertex*)
            realloc(debugLineData->verts, newVertsLength * sizeof(DebugLineVertex));
        vkDestroyBuffer(device, debugLineData->vertBuffer, NULL);
        vkFreeMemory(device, debugLineData->vertBufferMemory, NULL);
        createBuffer(
            device,
            physicalDevice,
            newVertsLength * sizeof(DebugLineVertex),
            0,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &debugLineData->vertBuffer,
            &debugLineData->vertBufferMemory);
    }
    memcpy(debugLineData->verts, newVerts, newVertsLength * sizeof(DebugLineVertex));
    copyDataToBuffer(
        device,
        newVerts,
        debugLineData->vertBufferMemory,
        0,
        newVertsLength * sizeof(DebugLineData));
}

void cleanupDebugLineData(
    VkDevice device,
    DebugLineData debugLineData)
{
    vkDestroyBuffer(device, debugLineData.vertBuffer, NULL);
    vkFreeMemory(device, debugLineData.vertBufferMemory, NULL);
    free(debugLineData.verts);
}
