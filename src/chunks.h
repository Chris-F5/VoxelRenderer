#ifndef CHUNKS_H
#define CHUNKS_H

#include "./bit_array.h"
#include "./id_allocator.h"
#include "./vox_palette.h"

#include <cglm/types.h>

#include <vulkan/vulkan.h>

#define CHUNK_SCALE 32
#define CHUNK_VOX_COUNT (CHUNK_SCALE * CHUNK_SCALE * CHUNK_SCALE)
#define CHUNK_BIT_MASK_SIZE ((CHUNK_VOX_COUNT + 7) / 8)

#define NEIGHBOUR_EMPTY ~0
#define NEIGHBOUR_NEGATIVE_X 0
#define NEIGHBOUR_NEGATIVE_Y 1
#define NEIGHBOUR_NEGATIVE_Z 2
#define NEIGHBOUR_POSITIVE_X 3
#define NEIGHBOUR_POSITIVE_Y 4
#define NEIGHBOUR_POSITIVE_Z 5

typedef uint32_t ChunkRef;

typedef struct {
    IdAllocator idAllocator;
    uint8_t (*voxBitMask)[CHUNK_BIT_MASK_SIZE];
    uint8_t (*colors)[CHUNK_VOX_COUNT];
    /**
     * 'neighbours' contains the neighbours of each chunk in the order:
     * negative x, negative y, negative z,
     * positive x, positive y, positive z
     * if a neighbour does not exist, then its value is 'NEIGHBOUR_EMPTY'
     */
    ChunkRef (*neighbours)[6];
    ivec3* positions;
} ChunkStorage;

typedef struct {
    uint32_t voxBitMaskChangesCapacity;
    uint32_t voxBitMaskChangesCount;
    uint32_t voxBitMaskChangesDupesCheckedUntil;
    ChunkRef* voxBitMaskChanges;
    uint32_t colorChangesCapacity;
    uint32_t colorChangesCount;
    uint32_t colorChangesDupesCheckedUntil;
    ChunkRef* colorChanges;
    uint32_t neighbourChangesCapacity;
    uint32_t neighbourChangesCount;
    uint32_t neighbourChangesDupesCheckedUntil;
    ChunkRef* neighbourChanges;
} ChunkStorageChanges;

typedef struct {
    VkBuffer voxBitMask;
    VkDeviceMemory voxBitMaskMemory;
    VkBuffer colors;
    VkDeviceMemory colorsMemory;
    VkBuffer normals;
    VkDeviceMemory normalsMemory;
    VkBuffer brightness;
    VkDeviceMemory brightnessMemory;
    VkBuffer neighbours;
    VkDeviceMemory neighboursMemory;
} ChunkGpuStorage;

/* CHUNK STORAGE METHODS */

void ChunkStorage_init(ChunkStorage* storage);

uint8_t* ChunkStorage_chunkBitMask(
    ChunkStorage* storage,
    ChunkRef chunk);

uint8_t* ChunkStorage_chunkColorData(
    ChunkStorage* storage,
    ChunkRef chunk);

void ChunkStorage_add(
    ChunkStorage* storage,
    ChunkStorageChanges* storageChanges,
    uint32_t count,
    ivec3* positions,
    ChunkRef* chunks);

void ChunkStorage_remove(
    ChunkStorage* storage,
    ChunkStorageChanges* storageChanges,
    uint32_t count,
    ChunkRef* chunks);

bool ChunkStorage_findChunkFromPos(
    ChunkStorage* storage,
    ivec3 pos,
    ChunkRef* chunk);

bool ChunkStorage_first(
    ChunkStorage* storage,
    ChunkRef* chunk);

bool ChunkStorage_next(
    ChunkStorage* storage,
    ChunkRef lastChunk,
    ChunkRef* chunk);

void ChunkStorage_destroy(
    ChunkStorage* storage);

/* CHUNK STORAGE CHANGES METHODS */

void ChunkStorageChanges_init(
    ChunkStorageChanges* storage,
    uint32_t voxBitMaskChangesCapacity,
    uint32_t colorChangesCapacity,
    uint32_t neighbourChangesCapacity);

void ChunkStorageChanges_addVoxBitMaskChanges(
    ChunkStorageChanges* storage,
    uint32_t chunksCount,
    ChunkRef* chunks);

void ChunkStorageChanges_addColorChanges(
    ChunkStorageChanges* storage,
    uint32_t chunksCount,
    ChunkRef* chunks);

void ChunkStorageChanges_addNeighbourChanges(
    ChunkStorageChanges* storage,
    uint32_t chunksCount,
    ChunkRef* chunks);

void ChunkStorageChanges_destroy(ChunkStorageChanges* storage);

/* CHUNK GPU STORAGE METHODS */

void ChunkGpuStorage_init(
    ChunkGpuStorage* storage,
    VkDevice logicalDevice,
    VkPhysicalDevice physicalDevice);

void ChunkGpuStorage_update(
    ChunkGpuStorage* gpuStorage,
    VkDevice logicalDevice,
    ChunkStorage* cpuStorage,
    ChunkStorageChanges* changes);

void ChunkGpuStorage_destroy(
    ChunkGpuStorage* storage,
    VkDevice logicalDevice);

#endif
