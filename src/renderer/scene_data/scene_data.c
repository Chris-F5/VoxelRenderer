#include "scene_data.h"

#include <stdlib.h>
#include <string.h>

#include <cglm/cglm.h>

#include "../vk_utils/buffer.h"
#include "../vk_utils/descriptor_set.h"
#include "../vk_utils/exceptions.h"
#include "mesh_gen.h"

const uint32_t VOX_BLOCK_SCALE = 2;
const uint32_t VOX_BLOCK_VOX_COUNT = VOX_BLOCK_SCALE * VOX_BLOCK_SCALE * VOX_BLOCK_SCALE;

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
    uint32_t maxDescriptorSetCount)
{
    VkDescriptorPoolSize poolSize;
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = maxDescriptorSetCount;

    VkDescriptorPoolCreateInfo poolCreateInfo;
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.pNext = NULL;
    poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolCreateInfo.maxSets = maxDescriptorSetCount;
    poolCreateInfo.poolSizeCount = 1;
    poolCreateInfo.pPoolSizes = &poolSize;

    VkDescriptorPool pool;
    handleVkResult(
        vkCreateDescriptorPool(device, &poolCreateInfo, NULL, &pool),
        "creating block descriptor pool");

    return pool;
}

SceneData createSceneData(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t maxBlockCount,
    uint32_t maxPaletteCount)
{
    SceneData sceneData;
    sceneData.maxBlockCount = maxBlockCount;
    sceneData.maxPaletteCount = maxPaletteCount;

    sceneData.allocatedBlocks = 0;
    sceneData.allocatedPalettes = 0;

    // VOXELS

    sceneData.blockVoxels = (Voxel*)malloc(
        maxBlockCount * VOX_BLOCK_VOX_COUNT * sizeof(Voxel));

    // PALETTES

    sceneData.paletteColors = (vec3*)malloc(
        maxPaletteCount * 256 * sizeof(vec3));

    // BLOCK INFO BUFFERS

    createBuffer(
        device,
        physicalDevice,
        maxBlockCount * sizeof(BlockDescriptorUniformBuffer),
        0,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        &sceneData.blocksInfoBuffer,
        &sceneData.blocksInfoBufferMemory);

    // DESCRIPTORS

    sceneData.blocksDescriptorPool = createBlockDescriptorPool(
        device,
        maxBlockCount);
    sceneData.blocksDescriptorSetLayout = createBlockDescriptorSetLayout(device);
    sceneData.blockDescriptorSets = (VkDescriptorSet*)malloc(
        maxBlockCount * sizeof(VkDescriptorSet));
    allocateDescriptorSets(
        device,
        sceneData.blocksDescriptorSetLayout,
        sceneData.blocksDescriptorPool,
        maxBlockCount,
        sceneData.blockDescriptorSets);

    for (int i = 0; i < maxBlockCount; i++) {
        VkDescriptorBufferInfo uniformBufferInfo;
        uniformBufferInfo.buffer = sceneData.blocksInfoBuffer;
        uniformBufferInfo.offset = i * sizeof(BlockDescriptorUniformBuffer);
        uniformBufferInfo.range = sizeof(BlockDescriptorUniformBuffer);

        VkWriteDescriptorSet uniformBufferDescriptorWrite;
        uniformBufferDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uniformBufferDescriptorWrite.pNext = NULL;
        uniformBufferDescriptorWrite.dstSet = sceneData.blockDescriptorSets[i];
        uniformBufferDescriptorWrite.dstBinding = 0;
        uniformBufferDescriptorWrite.dstArrayElement = 0;
        uniformBufferDescriptorWrite.descriptorCount = 1;
        uniformBufferDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferDescriptorWrite.pImageInfo = NULL;
        uniformBufferDescriptorWrite.pBufferInfo = &uniformBufferInfo;
        uniformBufferDescriptorWrite.pTexelBufferView = NULL;

        vkUpdateDescriptorSets(device, 1, &uniformBufferDescriptorWrite, 0, NULL);
    }

    // VERTEX BUFFERS

    sceneData.vertexBuffers = (VkBuffer*)malloc(
        maxBlockCount * sizeof(VkBuffer));
    sceneData.vertexBuffersMemory = (VkDeviceMemory*)malloc(
        maxBlockCount * sizeof(VkDeviceMemory));
    sceneData.vertexBuffersLength = (uint32_t*)malloc(
        maxBlockCount * sizeof(uint32_t));

    return sceneData;
}

PaletteRef createPalette(
    SceneData* sceneData,
    FILE* paletteFile)
{
    PaletteRef currentPalette = sceneData->allocatedPalettes++;
    vec3* palette = &sceneData->paletteColors[currentPalette * 256];

    unsigned char* rawColors = malloc(256 * 3);
    fread(rawColors, 3, 256, paletteFile);

    for (int i = 0; i < 256; i++) {
        palette[i][0] = (float)rawColors[i * 3 + 0] / 256.0f;
        palette[i][1] = (float)rawColors[i * 3 + 1] / 256.0f;
        palette[i][2] = (float)rawColors[i * 3 + 2] / 256.0f;
    }

    free(rawColors);

    return currentPalette;
}

BlockRef createEmptyBlock(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    SceneData* sceneData,
    vec3 pos,
    PaletteRef paletteRef)
{
    BlockRef blockRef;
    blockRef = sceneData->allocatedBlocks++;

    // VOXELS

    Voxel* blockVoxels = getBlockVoxels(sceneData, blockRef);
    memset(blockVoxels, 0, VOX_BLOCK_VOX_COUNT * sizeof(Voxel));

    // BLOCK INFO BUFFER

    BlockDescriptorUniformBuffer blockInfo;
    glm_mat4_identity(blockInfo.model);
    glm_translate(blockInfo.model, pos);
    copyDataToBuffer(
        device,
        &blockInfo,
        sceneData->blocksInfoBufferMemory,
        blockRef * sizeof(BlockDescriptorUniformBuffer),
        sizeof(BlockDescriptorUniformBuffer));

    // MESH

    createEmptyBlockVertexBuffer(
        device,
        physicalDevice,
        &sceneData->vertexBuffersLength[blockRef],
        &sceneData->vertexBuffers[blockRef],
        &sceneData->vertexBuffersMemory[blockRef]);

    return blockRef;
}

BlockRef createBlockFromFile(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    SceneData* sceneData,
    vec3 pos,
    PaletteRef paletteRef,
    FILE* blockFile)
{
    BlockRef block = createEmptyBlock(
        device,
        physicalDevice,
        sceneData,
        pos,
        paletteRef);

    // VOXELS

    Voxel* blockVoxels = getBlockVoxels(sceneData, block);
    fread(blockVoxels, sizeof(Voxel), VOX_BLOCK_VOX_COUNT, blockFile);

    // MESH

    writeBlockVertexBuffer(
        device,
        blockVoxels,
        &sceneData->paletteColors[paletteRef * 256],
        &sceneData->vertexBuffersLength[block],
        &sceneData->vertexBuffers[block],
        &sceneData->vertexBuffersMemory[block]);

    return block;
}

Voxel* getBlockVoxels(
    SceneData* sceneData,
    BlockRef block)
{
    return &sceneData->blockVoxels[block * VOX_BLOCK_VOX_COUNT];
}

void updateBlockVertexBuffer(
    VkDevice device,
    SceneData* sceneData,
    PaletteRef palette,
    BlockRef block)
{
    writeBlockVertexBuffer(
        device,
        getBlockVoxels(sceneData, block),
        &sceneData->paletteColors[palette * 256],
        &sceneData->vertexBuffersLength[block],
        &sceneData->vertexBuffers[block],
        &sceneData->vertexBuffersMemory[block]);
}

void cleanupSceneData(VkDevice device, SceneData sceneData)
{
    free(sceneData.blockVoxels);

    free(sceneData.paletteColors);

    vkDestroyBuffer(device, sceneData.blocksInfoBuffer, NULL);
    vkFreeMemory(device, sceneData.blocksInfoBufferMemory, NULL);

    vkDestroyDescriptorPool(device, sceneData.blocksDescriptorPool, NULL);
    vkDestroyDescriptorSetLayout(device, sceneData.blocksDescriptorSetLayout, NULL);
    free(sceneData.blockDescriptorSets);

    for (int i = 0; i < sceneData.allocatedBlocks; i++) {
        vkDestroyBuffer(device, sceneData.vertexBuffers[i], NULL);
        vkFreeMemory(device, sceneData.vertexBuffersMemory[i], NULL);
    }
    free(sceneData.vertexBuffersLength);
}
