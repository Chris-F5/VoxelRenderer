#include "block.h"

#include <stdlib.h>
#include <string.h>

#include <cglm/cglm.h>

#include "../vk_utils/buffer.h"
#include "../vk_utils/descriptor_set.h"
#include "../vk_utils/exceptions.h"

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
    return voxels[i].color[0] != 0 || voxels[i].color[1] != 0 || voxels[i].color[2] != 0;
}

void createBlockVertices(
    const Voxel* voxels,
    uint32_t* vertexCount,
    Vertex** vertices)
{
    const uint32_t maxVertexCount = VOX_BLOCK_VOX_COUNT * 8;
    *vertices = (Vertex*)malloc(maxVertexCount * sizeof(Vertex));

    *vertexCount = 0;

    for (int i = 0; i < VOX_BLOCK_VOX_COUNT; i++)
        if (voxels[i].color[0] != 0 || voxels[i].color[1] != 0 || voxels[i].color[2] != 0) {
            int x = i % VOX_BLOCK_SCALE;
            int y = i / VOX_BLOCK_SCALE % VOX_BLOCK_SCALE;
            int z = i / (VOX_BLOCK_SCALE * VOX_BLOCK_SCALE);

            if (!isVoxelPresent(x + 1, y + 0, z + 0, voxels))
                addFace(x, y, z, FACE_POINTS_XP, voxels[i].color, vertexCount, *vertices);
            if (!isVoxelPresent(x - 1, y + 0, z + 0, voxels))
                addFace(x, y, z, FACE_POINTS_XN, voxels[i].color, vertexCount, *vertices);
            if (!isVoxelPresent(x + 0, y + 1, z + 0, voxels))
                addFace(x, y, z, FACE_POINTS_YP, voxels[i].color, vertexCount, *vertices);
            if (!isVoxelPresent(x + 0, y - 1, z + 0, voxels))
                addFace(x, y, z, FACE_POINTS_YN, voxels[i].color, vertexCount, *vertices);
            if (!isVoxelPresent(x + 0, y + 0, z + 1, voxels))
                addFace(x, y, z, FACE_POINTS_ZP, voxels[i].color, vertexCount, *vertices);
            if (!isVoxelPresent(x + 0, y + 0, z - 1, voxels))
                addFace(x, y, z, FACE_POINTS_ZN, voxels[i].color, vertexCount, *vertices);
        }
}

VkDescriptorSetLayout createBlockDescriptorSetLayout(VkDevice device)
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
        "creating block descriptor set layout");

    return layout;
}

VkDescriptorPool createBlockDescriptorPool(
    VkDevice device,
    uint32_t swapchainImageCount,
    uint32_t maxBlockCount)
{
    VkDescriptorPoolSize poolSize;
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = maxBlockCount * swapchainImageCount;

    VkDescriptorPoolCreateInfo poolCreateInfo;
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.pNext = NULL;
    poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolCreateInfo.maxSets = maxBlockCount * swapchainImageCount;
    poolCreateInfo.poolSizeCount = 1;
    poolCreateInfo.pPoolSizes = &poolSize;

    VkDescriptorPool pool;
    handleVkResult(
        vkCreateDescriptorPool(device, &poolCreateInfo, NULL, &pool),
        "creating block descriptor pool");

    return pool;
}

Block createBlock(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkDescriptorSetLayout blockDescriptorSetLayout,
    VkDescriptorPool descriptorPool,
    uint32_t swapchainImageCount,
    Voxel* voxels)
{
    Block block;
    block.voxels = (Voxel*)malloc(VOX_BLOCK_VOX_COUNT * sizeof(Voxel));
    memcpy(block.voxels, voxels, VOX_BLOCK_VOX_COUNT * sizeof(Voxel));

    glm_mat4_identity(block.descriptorData.model);

    // ALLOCATE DESCRIPTOR SETS

    block.descriptorSets = (VkDescriptorSet*)malloc(swapchainImageCount * sizeof(VkDescriptorSet));
    allocateDescriptorSets(
        device,
        blockDescriptorSetLayout,
        descriptorPool,
        swapchainImageCount,
        block.descriptorSets);

    // CREATE DESCRIPTOR BUFFERS

    block.descriptorSetUniformBuffers = (VkBuffer*)malloc(swapchainImageCount * sizeof(VkBuffer));
    block.descriptorSetUniformBuffersMemory = (VkDeviceMemory*)malloc(swapchainImageCount * sizeof(VkDeviceMemory));
    createUniformBuffers(
        device,
        physicalDevice,
        sizeof(BlockDescriptorUniformBuffer),
        swapchainImageCount,
        block.descriptorSetUniformBuffers,
        block.descriptorSetUniformBuffersMemory);

    // BIND BUFFERS TO DESCRIPTOR SETS

    for (int i = 0; i < swapchainImageCount; i++) {
        VkDescriptorBufferInfo uniformBufferInfo;
        uniformBufferInfo.buffer = block.descriptorSetUniformBuffers[i];
        uniformBufferInfo.offset = 0;
        uniformBufferInfo.range = sizeof(BlockDescriptorUniformBuffer);

        VkWriteDescriptorSet uniformBufferDescriptorWrite;
        uniformBufferDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uniformBufferDescriptorWrite.pNext = NULL;
        uniformBufferDescriptorWrite.dstSet = block.descriptorSets[i];
        uniformBufferDescriptorWrite.dstBinding = 0;
        uniformBufferDescriptorWrite.dstArrayElement = 0;
        uniformBufferDescriptorWrite.descriptorCount = 1;
        uniformBufferDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferDescriptorWrite.pImageInfo = NULL;
        uniformBufferDescriptorWrite.pBufferInfo = &uniformBufferInfo;
        uniformBufferDescriptorWrite.pTexelBufferView = NULL;

        vkUpdateDescriptorSets(device, 1, &uniformBufferDescriptorWrite, 0, NULL);
    }

    // VERTEX BUFFER

    Vertex* vertices;
    createBlockVertices(
        block.voxels,
        &block.vertexBufferLength,
        &vertices);

    createBuffer(
        device,
        physicalDevice,
        block.vertexBufferLength * sizeof(Vertex),
        0,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &block.vertexBuffer,
        &block.vertexBufferMemory);

    copyDataToBuffer(
        device,
        (void*)vertices,
        block.vertexBufferMemory,
        0,
        block.vertexBufferLength * sizeof(Vertex));

    return block;
}

void updateBlockDescriptors(VkDevice device, Block* block, uint32_t currentSwapchainImageIndex)
{
    copyDataToBuffer(
        device,
        &block->descriptorData,
        block->descriptorSetUniformBuffersMemory[currentSwapchainImageIndex],
        0,
        sizeof(BlockDescriptorUniformBuffer));
}

void cleanupBlock(
    VkDevice device,
    Block block,
    uint32_t swapchainImageCount,
    VkDescriptorPool blockDescriptorPool)
{
    vkFreeDescriptorSets(device, blockDescriptorPool, swapchainImageCount, block.descriptorSets);
    for (int i = 0; i < swapchainImageCount; i++) {
        vkDestroyBuffer(device, block.descriptorSetUniformBuffers[i], NULL);
        vkFreeMemory(device, block.descriptorSetUniformBuffersMemory[i], NULL);
    }
    vkDestroyBuffer(device, block.vertexBuffer, NULL);
    vkFreeMemory(device, block.vertexBufferMemory, NULL);

    free(block.voxels);
    free(block.descriptorSets);
    free(block.descriptorSetUniformBuffers);
    free(block.descriptorSetUniformBuffersMemory);
}
