#include "scene_data.h"

#include <string.h>

#include "buffer.h"

const Vertex VERTICES[] = {{{0.1f, -0.6f}, {1.0f, 0.0f, 0.0f}},
                           {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                           {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
const size_t VERTEX_COUNT = sizeof(VERTICES) / sizeof(VERTICES[0]);

const VkVertexInputBindingDescription VERTEX_BINDING_DESCRIPTIONS[] = {
    {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};
const size_t VERTEX_BINDING_DESCRIPTION_COUNT = sizeof(VERTEX_BINDING_DESCRIPTIONS) / sizeof(VERTEX_BINDING_DESCRIPTIONS[0]);

const VkVertexInputAttributeDescription VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[] = {
    {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, pos)},
    {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)}};
const size_t VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT = sizeof(VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS) / sizeof(VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[0]);

VkBuffer createVertexBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkBuffer *buffer,
    VkDeviceMemory *bufferMemory)
{
    createBuffer(
        device,
        physicalDevice,
        sizeof(VERTICES),
        0,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        buffer,
        bufferMemory);

    void *vertexData;
    vkMapMemory(device, *bufferMemory, 0, sizeof(VERTICES), 0, &vertexData);
    memcpy(vertexData, VERTICES, sizeof(VERTICES));
    vkUnmapMemory(device, *bufferMemory);
}
