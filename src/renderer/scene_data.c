#include "scene_data.h"

#include <string.h>

#include "vk_utils/buffer.h"

const Vertex VERTICES[] = {
    { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
    { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
    { { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
    { { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } }
};
const size_t VERTEX_COUNT = sizeof(VERTICES) / sizeof(VERTICES[0]);

const uint32_t VERTEX_INDICES[] = { 0, 1, 2, 2, 3, 0 };
const size_t VERTEX_INDEX_COUNT = sizeof(VERTEX_INDICES) / sizeof(VERTEX_INDICES[0]);

const VkVertexInputBindingDescription VERTEX_BINDING_DESCRIPTIONS[] = {
    { 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX }
};
const size_t VERTEX_BINDING_DESCRIPTION_COUNT = sizeof(VERTEX_BINDING_DESCRIPTIONS) / sizeof(VERTEX_BINDING_DESCRIPTIONS[0]);

const VkVertexInputAttributeDescription VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[] = {
    { 0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, pos) },
    { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) }
};
const size_t VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT = sizeof(VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS) / sizeof(VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[0]);

void createVertexBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkQueue queue,
    VkCommandPool commandPool,
    VkBuffer* stagingBuffer,
    VkDeviceMemory* stagingBufferMemory,
    VkBuffer* vertexBuffer,
    VkDeviceMemory* vertexBufferMemory)
{
    createBuffer(
        device,
        physicalDevice,
        sizeof(VERTICES),
        0,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory);

    createBuffer(
        device,
        physicalDevice,
        sizeof(VERTICES),
        0,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vertexBuffer,
        vertexBufferMemory);

    copyDataToBuffer(
        device,
        (void*)VERTICES,
        *stagingBufferMemory,
        0,
        sizeof(VERTICES));

    VkBufferCopy copyRegion;
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = sizeof(VERTICES);
    bufferTransfer(device, queue, commandPool, 1, &copyRegion, *stagingBuffer, *vertexBuffer);
}

void createIndexBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkQueue queue,
    VkCommandPool commandPool,
    VkBuffer* stagingBuffer,
    VkDeviceMemory* stagingBufferMemory,
    VkBuffer* indexBuffer,
    VkDeviceMemory* indexBufferMemory)
{
    createBuffer(
        device,
        physicalDevice,
        sizeof(VERTEX_INDICES),
        0,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory);

    createBuffer(
        device,
        physicalDevice,
        sizeof(VERTEX_INDICES),
        0,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        indexBuffer,
        indexBufferMemory);

    copyDataToBuffer(
        device,
        (void*)VERTEX_INDICES,
        *stagingBufferMemory,
        0,
        sizeof(VERTEX_INDICES));

    VkBufferCopy copyRegion;
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = sizeof(VERTEX_INDICES);
    bufferTransfer(device, queue, commandPool, 1, &copyRegion, *stagingBuffer, *indexBuffer);
}
