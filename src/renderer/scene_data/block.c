#include "block.h"

#include <stdlib.h>

#include <cglm/cglm.h>

#include "../vk_utils/buffer.h"

typedef uint32_t VertexIndex;

const VertexIndex INDEX_BUFFER[] = {
    0, 1, 2,
    1, 3, 2,
    4, 1, 0,
    4, 5, 1
};
const VkVertexInputBindingDescription VERTEX_BINDING_DESCRIPTIONS[] = {
    { 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX }
};
const size_t VERTEX_BINDING_DESCRIPTION_COUNT = sizeof(VERTEX_BINDING_DESCRIPTIONS) / sizeof(VERTEX_BINDING_DESCRIPTIONS[0]);

const VkVertexInputAttributeDescription VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[] = {
    { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos) },
    { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) }
};
const size_t VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT = sizeof(VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS) / sizeof(VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[0]);

const uint32_t VOX_BLOCK_SCALE = 3;
const uint32_t VOX_BLOCK_VOX_COUNT = VOX_BLOCK_SCALE * VOX_BLOCK_SCALE * VOX_BLOCK_SCALE;

const vec3 FACE_POINTS_XP[] = {
    { 1, 0, 0 },
    { 1, 1, 0 },
    { 1, 0, 1 },

    { 1, 1, 0 },
    { 1, 1, 1 },
    { 1, 0, 1 }
};
const vec3 FACE_POINTS_XN[] = {
    { 0, 0, 0 },
    { 0, 0, 1 },
    { 0, 1, 0 },

    { 0, 1, 0 },
    { 0, 0, 1 },
    { 0, 1, 1 }
};
const vec3 FACE_POINTS_YP[] = {
    { 0, 1, 0 },
    { 0, 1, 1 },
    { 1, 1, 0 },

    { 1, 1, 0 },
    { 0, 1, 1 },
    { 1, 1, 1 }
};
const vec3 FACE_POINTS_YN[] = {
    { 0, 0, 0 },
    { 1, 0, 0 },
    { 0, 0, 1 },

    { 1, 0, 0 },
    { 1, 0, 1 },
    { 0, 0, 1 }
};
const vec3 FACE_POINTS_ZP[] = {
    { 0, 0, 1 },
    { 1, 0, 1 },
    { 0, 1, 1 },

    { 1, 0, 1 },
    { 1, 1, 1 },
    { 0, 1, 1 }
};
const vec3 FACE_POINTS_ZN[] = {
    { 0, 0, 0 },
    { 0, 1, 0 },
    { 1, 0, 0 },

    { 1, 0, 0 },
    { 0, 1, 0 },
    { 1, 1, 0 }
};

void createVertexBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkQueue queue,
    VkCommandPool commandPool,
    uint32_t vertexCount,
    Vertex* vertices,
    VkBuffer* vertexBuffer,
    VkDeviceMemory* vertexBufferMemory)
{
    createBuffer(
        device,
        physicalDevice,
        vertexCount * sizeof(Vertex),
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
        vertexCount * sizeof(Vertex),
        0,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &stagingBuffer,
        &stagingBufferMemory);

    copyDataToBuffer(
        device,
        (void*)vertices,
        stagingBufferMemory,
        0,
        vertexCount * sizeof(Vertex));

    VkBufferCopy copyRegion;
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = vertexCount * sizeof(Vertex);

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

void createIndexBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t indexCount,
    VertexIndex* indices,
    VkBuffer* indexBuffer,
    VkDeviceMemory* indexBufferMemory)
{
    createBuffer(
        device,
        physicalDevice,
        indexCount * sizeof(VertexIndex),
        0,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        indexBuffer,
        indexBufferMemory);

    copyDataToBuffer(
        device,
        (void*)indices,
        *indexBufferMemory,
        0,
        indexCount * sizeof(VertexIndex));
}

void setVertex(
    vec3 pos,
    const vec3 color,
    Vertex* vertex)
{
    glm_vec3_copy(pos, vertex->pos);
    vertex->color[0] = color[0];
    vertex->color[1] = color[1];
    vertex->color[2] = color[2];
}

void setVertices(
    uint32_t count,
    vec3* positions,
    const vec3 color,
    uint32_t* vertexCount,
    Vertex* vertices)
{
    for (int i = 0; i < count; i++) {
        setVertex(positions[i], color, &vertices[*vertexCount]);
        *vertexCount += 1;
    }
}

void addFace(
    int x,
    int y,
    int z,
    const vec3* facePoints,
    const vec3 color,
    uint32_t* vertexCount,
    Vertex* vertices,
    uint32_t* indexCount,
    VertexIndex* indices)
{
    for (int i = 0; i < 6; i++) {
        vec3 pos;
        vec3 point = { facePoints[i][0], facePoints[i][1], facePoints[i][2] };
        glm_vec3_add(point, (vec3) { x, y, z }, pos);
        setVertex(pos, color, &vertices[*vertexCount]);
        *vertexCount += 1;
        indices[*indexCount] = *indexCount;
        *indexCount += 1;
    }
}

bool isBlockPresent(
    uint32_t x,
    uint32_t y,
    uint32_t z,
    const Voxel* block)
{
    if (x >= VOX_BLOCK_SCALE || x < 0)
        return false;
    if (y >= VOX_BLOCK_SCALE || y < 0)
        return false;
    if (z >= VOX_BLOCK_SCALE || z < 0)
        return false;

    uint32_t i = x + y * VOX_BLOCK_SCALE + z * VOX_BLOCK_SCALE * VOX_BLOCK_SCALE;
    return block[i].color[0] != 0 || block[i].color[1] != 0 || block[i].color[2] != 0;
}

void createBlockVertices(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkQueue queue,
    VkCommandPool commandPool,
    const Voxel* block,
    uint32_t* vertexCount,
    VkBuffer* vertexBuffer,
    VkDeviceMemory* vertexBufferMemory,
    uint32_t* indexCount,
    VkBuffer* indexBuffer,
    VkDeviceMemory* indexBufferMemory)
{
    const uint32_t maxVertexCount = VOX_BLOCK_VOX_COUNT * 8;
    Vertex* vertices = (Vertex*)malloc(maxVertexCount * sizeof(Vertex));
    VertexIndex* indices = (VertexIndex*)malloc(maxVertexCount * sizeof(VertexIndex));

    *vertexCount = 0;
    *indexCount = 0;

    for (int i = 0; i < VOX_BLOCK_VOX_COUNT; i++)
        if (block[i].color[0] != 0 || block[i].color[1] != 0 || block[i].color[2] != 0) {
            int x = i % VOX_BLOCK_SCALE;
            int y = i / VOX_BLOCK_SCALE % VOX_BLOCK_SCALE;
            int z = i / (VOX_BLOCK_SCALE * VOX_BLOCK_SCALE);

            if (!isBlockPresent(x + 1, y + 0, z + 0, block))
                addFace(x, y, z, FACE_POINTS_XP, block[i].color, vertexCount, vertices, indexCount, indices);
            if (!isBlockPresent(x - 1, y + 0, z + 0, block))
                addFace(x, y, z, FACE_POINTS_XN, block[i].color, vertexCount, vertices, indexCount, indices);
            if (!isBlockPresent(x + 0, y + 1, z + 0, block))
                addFace(x, y, z, FACE_POINTS_YP, block[i].color, vertexCount, vertices, indexCount, indices);
            if (!isBlockPresent(x + 0, y - 1, z + 0, block))
                addFace(x, y, z, FACE_POINTS_YN, block[i].color, vertexCount, vertices, indexCount, indices);
            if (!isBlockPresent(x + 0, y + 0, z + 1, block))
                addFace(x, y, z, FACE_POINTS_ZP, block[i].color, vertexCount, vertices, indexCount, indices);
            if (!isBlockPresent(x + 0, y + 0, z - 1, block))
                addFace(x, y, z, FACE_POINTS_ZN, block[i].color, vertexCount, vertices, indexCount, indices);
        }

    createVertexBuffer(
        device,
        physicalDevice,
        queue,
        commandPool,
        *vertexCount,
        vertices,
        vertexBuffer,
        vertexBufferMemory);

    createIndexBuffer(
        device,
        physicalDevice,
        *indexCount,
        indices,
        indexBuffer,
        indexBufferMemory);
}
