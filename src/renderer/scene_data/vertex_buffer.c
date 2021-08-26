#include "vertex_buffer.h"

#include <string.h>
#include <stdlib.h>

#include <cglm/types.h>
#include <vulkan/vulkan_core.h>

#include "../vk_utils/buffer.h"

const VkVertexInputBindingDescription VERTEX_BINDING_DESCRIPTIONS[] = {
    { 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX }
};
const size_t VERTEX_BINDING_DESCRIPTION_COUNT = sizeof(VERTEX_BINDING_DESCRIPTIONS) / sizeof(VERTEX_BINDING_DESCRIPTIONS[0]);

const VkVertexInputAttributeDescription VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[] = {
    { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos) }
};
const size_t VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT = sizeof(VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS) / sizeof(VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[0]);

void createVertexGrid(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkQueue queue,
    VkCommandPool commandPool,
    uint32_t gridSize,
    uint32_t* vertexCount,
    VkBuffer* vertexBuffer,
    VkDeviceMemory* vertexBufferMemory)
{
    *vertexCount = gridSize * gridSize * gridSize;

    createBuffer(
        device,
        physicalDevice,
        *vertexCount * sizeof(Vertex),
        0,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vertexBuffer,
        vertexBufferMemory);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(
        device,
        physicalDevice,
        *vertexCount * sizeof(Vertex),
        0,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &stagingBuffer,
        &stagingBufferMemory);

    Vertex* vertexGrid;
    vertexGrid = (Vertex*)malloc(*vertexCount * sizeof(Vertex));

    for (uint32_t x = 0; x < gridSize; x++)
        for (uint32_t y = 0; y < gridSize; y++)
            for (uint32_t z = 0; z < gridSize; z++) 
                vertexGrid[x + y * gridSize + z * gridSize * gridSize] = (Vertex){ { (float)x, (float)y, (float)z } };

    copyDataToBuffer(
        device,
        (void*)vertexGrid,
        stagingBufferMemory,
        0,
        *vertexCount * sizeof(Vertex));

    free(vertexGrid);

    VkBufferCopy copyRegion;
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = *vertexCount * sizeof(Vertex);

    bufferTransfer(
        device,
        queue,
        commandPool,
        1,
        &copyRegion,
        stagingBuffer,
        *vertexBuffer);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);
}
