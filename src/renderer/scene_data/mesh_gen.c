#include "mesh_gen.h"

#include <cglm/cglm.h>

#include "../vk_utils/buffer.h"

typedef struct {
    vec3 pos;
    vec3 color;
} Vertex;

const VkVertexInputBindingDescription MODEL_VERTEX_BINDING_DESCRIPTIONS[] = {
    { 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX }
};
const size_t MODEL_VERTEX_BINDING_DESCRIPTION_COUNT
    = sizeof(MODEL_VERTEX_BINDING_DESCRIPTIONS) / sizeof(MODEL_VERTEX_BINDING_DESCRIPTIONS[0]);

const VkVertexInputAttributeDescription MODEL_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[] = {
    { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos) },
    { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) }
};
const size_t MODEL_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT
    = sizeof(MODEL_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS) / sizeof(MODEL_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[0]);

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

void setVertex(
    const vec3 pos,
    const vec3 color,
    Vertex* vertex)
{
    vertex->pos[0] = pos[0];
    vertex->pos[1] = pos[1];
    vertex->pos[2] = pos[2];
    vertex->color[0] = color[0];
    vertex->color[1] = color[1];
    vertex->color[2] = color[2];
}

void addFace(
    int x,
    int y,
    int z,
    const vec3* facePoints,
    const vec3 color,
    uint32_t* vertexCount,
    Vertex* vertices)
{
    for (int i = 0; i < 6; i++) {
        vec3 pos;
        vec3 point = { facePoints[i][0], facePoints[i][1], facePoints[i][2] };
        glm_vec3_add(point, (vec3) { x, y, z }, pos);
        setVertex(pos, color, &vertices[*vertexCount]);
        *vertexCount += 1;
    }
}

bool isVoxelPresent(
    uint32_t x,
    uint32_t y,
    uint32_t z,
    const Voxel* voxels)
{
    if (x >= VOX_BLOCK_SCALE || x < 0)
        return false;
    if (y >= VOX_BLOCK_SCALE || y < 0)
        return false;
    if (z >= VOX_BLOCK_SCALE || z < 0)
        return false;

    uint32_t i = x + y * VOX_BLOCK_SCALE + z * VOX_BLOCK_SCALE * VOX_BLOCK_SCALE;
    return voxels[i] != 0;
}

void createBlockMesh(
    const Voxel* voxels,
    const vec3* palette,
    uint32_t* vertexCount,
    Vertex* vertices)
{
    *vertexCount = 0;

    for (int i = 0; i < VOX_BLOCK_VOX_COUNT; i++)
        if (voxels[i] != 0) {
            vec3 color;
            color[0] = palette[voxels[i]][0];
            color[1] = palette[voxels[i]][1];
            color[2] = palette[voxels[i]][2];

            int x = i % VOX_BLOCK_SCALE;
            int y = i / VOX_BLOCK_SCALE % VOX_BLOCK_SCALE;
            int z = i / (VOX_BLOCK_SCALE * VOX_BLOCK_SCALE);

            if (!isVoxelPresent(x + 1, y + 0, z + 0, voxels))
                addFace(x, y, z, FACE_POINTS_XP, color, vertexCount, vertices);
            if (!isVoxelPresent(x - 1, y + 0, z + 0, voxels))
                addFace(x, y, z, FACE_POINTS_XN, color, vertexCount, vertices);
            if (!isVoxelPresent(x + 0, y + 1, z + 0, voxels))
                addFace(x, y, z, FACE_POINTS_YP, color, vertexCount, vertices);
            if (!isVoxelPresent(x + 0, y - 1, z + 0, voxels))
                addFace(x, y, z, FACE_POINTS_YN, color, vertexCount, vertices);
            if (!isVoxelPresent(x + 0, y + 0, z + 1, voxels))
                addFace(x, y, z, FACE_POINTS_ZP, color, vertexCount, vertices);
            if (!isVoxelPresent(x + 0, y + 0, z - 1, voxels))
                addFace(x, y, z, FACE_POINTS_ZN, color, vertexCount, vertices);
        }
}

void createEmptyBlockVertexBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t* vertexBufferLength,
    VkBuffer* vertexBuffer,
    VkDeviceMemory* vertexBufferMemory)
{
    *vertexBufferLength = 0;
    const uint32_t maxVertexCount = VOX_BLOCK_VOX_COUNT * 18;
    createBuffer(
        device,
        physicalDevice,
        maxVertexCount * sizeof(Vertex),
        0,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vertexBuffer,
        vertexBufferMemory);
}

void writeBlockVertexBuffer(
    VkDevice device,
    const Voxel* voxels,
    const vec3* palette,
    uint32_t* vertexBufferLength,
    VkBuffer* vertexBuffer,
    VkDeviceMemory* vertexBufferMemory)
{
    if (*vertexBuffer == VK_NULL_HANDLE) {
        puts("Cant write block vertices to null vertex buffer. Exiting.");
        exit(EXIT_FAILURE);
    }
    const uint32_t maxVertexCount = VOX_BLOCK_VOX_COUNT * 18;
    Vertex* vertices = (Vertex*)malloc(maxVertexCount * sizeof(Vertex));
    createBlockMesh(
        voxels,
        palette,
        vertexBufferLength,
        vertices);

    if (*vertexBufferLength != 0)
        copyDataToBuffer(
            device,
            (void*)vertices,
            *vertexBufferMemory,
            0,
            *vertexBufferLength * sizeof(Vertex));

    free(vertices);
}
