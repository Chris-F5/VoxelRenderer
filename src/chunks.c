#include "./chunks.h"

#include <stdlib.h>
#include <string.h>

#include <cglm/cglm.h>

#include "./vert_gen.h"
#include "./vk_utils/buffer.h"

const ChunkRef CHUNK_NEIGHBOUR_EMPTY = ~0;
const int CHUNK_NEGATIVE_X_NEIGHBOUR = 0;
const int CHUNK_NEGATIVE_Y_NEIGHBOUR = 1;
const int CHUNK_NEGATIVE_Z_NEIGHBOUR = 2;
const int CHUNK_POSITIVE_X_NEIGHBOUR = 3;
const int CHUNK_POSITIVE_Y_NEIGHBOUR = 4;
const int CHUNK_POSITIVE_Z_NEIGHBOUR = 5;

static const unsigned int CHUNK_CAPACITY = 10000;

/* CHUNK STORAGE METHODS */

void ChunkStorage_init(ChunkStorage* storage)
{
    IdAllocator_init(&storage->idAllocator, CHUNK_CAPACITY);
    storage->voxBitMask = (uint8_t(*)[CHUNK_BIT_MASK_SIZE])malloc(
        CHUNK_CAPACITY * sizeof(uint8_t[CHUNK_BIT_MASK_SIZE]));
    storage->colors = (uint8_t(*)[CHUNK_VOX_COUNT])malloc(
        CHUNK_CAPACITY * sizeof(uint8_t[CHUNK_VOX_COUNT]));
    storage->neighbours = (ChunkRef(*)[6])malloc(
        CHUNK_CAPACITY * sizeof(ChunkRef[6]));
    storage->positions = (ivec3*)malloc(
        CHUNK_CAPACITY * sizeof(ivec3));
}

inline uint8_t* ChunkStorage_chunkBitMask(
    ChunkStorage* storage,
    ChunkRef chunk)
{
    return storage->voxBitMask[chunk];
}

inline uint8_t* ChunkStorage_chunkColorData(
    ChunkStorage* storage,
    ChunkRef chunk)
{
    return storage->colors[chunk];
}

void ChunkStorage_add(
    ChunkStorage* storage,
    ChunkStorageChanges* storageChanges,
    uint32_t count,
    ivec3* positions,
    ChunkRef* chunks)
{
    IdAllocator_allocate(
        &storage->idAllocator,
        count,
        chunks);
    for (uint32_t i = 0; i < count; i++) {
        printf("%d\n", chunks[i]);
        memset(&storage->voxBitMask[chunks[i]], 0, CHUNK_BIT_MASK_SIZE);
        memset(storage->colors[chunks[i]], 0, CHUNK_VOX_COUNT);
        for (int n = 0; n < 6; n++)
            storage->neighbours[chunks[i]][n] = CHUNK_NEIGHBOUR_EMPTY;
        storage->positions[chunks[i]][0] = positions[i][0];
        storage->positions[chunks[i]][1] = positions[i][1];
        storage->positions[chunks[i]][2] = positions[i][2];
    }
    ChunkStorageChanges_addColorChanges(
        storageChanges,
        count,
        chunks);
    ChunkStorageChanges_addVoxBitMaskChanges(
        storageChanges,
        count,
        chunks);

    for (uint32_t i = 0; i < count; i++) {
        { /* NEGATIVE X NEIGHBOUR */
            ivec3 pos;
            pos[0] = positions[i][0] - 1;
            pos[1] = positions[i][1];
            pos[2] = positions[i][2];
            ChunkRef neighbour;
            if (ChunkStorage_findChunkFromPos(storage, pos, &neighbour)) {
                storage->neighbours[chunks[i]][CHUNK_NEGATIVE_X_NEIGHBOUR]
                    = neighbour;
                storage->neighbours[neighbour][CHUNK_POSITIVE_X_NEIGHBOUR]
                    = chunks[i];
                ChunkStorageChanges_addNeighbourChanges(
                    storageChanges,
                    1,
                    &neighbour);
            }
        }
        { /* NEGATIVE Y NEIGHBOUR */
            ivec3 pos;
            pos[0] = positions[i][0];
            pos[1] = positions[i][1] - 1;
            pos[2] = positions[i][2];
            ChunkRef neighbour;
            if (ChunkStorage_findChunkFromPos(storage, pos, &neighbour)) {
                storage->neighbours[chunks[i]][CHUNK_NEGATIVE_Y_NEIGHBOUR]
                    = neighbour;
                storage->neighbours[neighbour][CHUNK_POSITIVE_Y_NEIGHBOUR]
                    = chunks[i];
                ChunkStorageChanges_addNeighbourChanges(
                    storageChanges,
                    1,
                    &neighbour);
            }
        }
        { /* NEGATIVE Z NEIGHBOUR */
            ivec3 pos;
            pos[0] = positions[i][0];
            pos[1] = positions[i][1];
            pos[2] = positions[i][2] - 1;
            ChunkRef neighbour;
            if (ChunkStorage_findChunkFromPos(storage, pos, &neighbour)) {
                storage->neighbours[chunks[i]][CHUNK_NEGATIVE_Z_NEIGHBOUR]
                    = neighbour;
                storage->neighbours[neighbour][CHUNK_POSITIVE_Z_NEIGHBOUR]
                    = chunks[i];
                ChunkStorageChanges_addNeighbourChanges(
                    storageChanges,
                    1,
                    &neighbour);
            }
        }
        { /* POSITIVE X NEIGHBOUR */
            ivec3 pos;
            pos[0] = positions[i][0] + 1;
            pos[1] = positions[i][1];
            pos[2] = positions[i][2];
            ChunkRef neighbour;
            if (ChunkStorage_findChunkFromPos(storage, pos, &neighbour)) {
                storage->neighbours[chunks[i]][CHUNK_POSITIVE_X_NEIGHBOUR]
                    = neighbour;
                storage->neighbours[neighbour][CHUNK_NEGATIVE_X_NEIGHBOUR]
                    = chunks[i];
                ChunkStorageChanges_addNeighbourChanges(
                    storageChanges,
                    1,
                    &neighbour);
            }
        }
        { /* POSITIVE Y NEIGHBOUR */
            ivec3 pos;
            pos[0] = positions[i][0];
            pos[1] = positions[i][1] + 1;
            pos[2] = positions[i][2];
            ChunkRef neighbour;
            if (ChunkStorage_findChunkFromPos(storage, pos, &neighbour)) {
                storage->neighbours[chunks[i]][CHUNK_POSITIVE_Y_NEIGHBOUR]
                    = neighbour;
                storage->neighbours[neighbour][CHUNK_NEGATIVE_Y_NEIGHBOUR]
                    = chunks[i];
                ChunkStorageChanges_addNeighbourChanges(
                    storageChanges,
                    1,
                    &neighbour);
            }
        }
        { /* POSITIVE Z NEIGHBOUR */
            ivec3 pos;
            pos[0] = positions[i][0];
            pos[1] = positions[i][1];
            pos[2] = positions[i][2] + 1;
            ChunkRef neighbour;
            if (ChunkStorage_findChunkFromPos(storage, pos, &neighbour)) {
                storage->neighbours[chunks[i]][CHUNK_POSITIVE_Z_NEIGHBOUR]
                    = neighbour;
                storage->neighbours[neighbour][CHUNK_NEGATIVE_Z_NEIGHBOUR]
                    = chunks[i];
                ChunkStorageChanges_addNeighbourChanges(
                    storageChanges,
                    1,
                    &neighbour);
            }
        }
    }

    ChunkStorageChanges_addNeighbourChanges(
        storageChanges,
        count,
        chunks);
}

void ChunkStorage_remove(
    ChunkStorage* storage,
    ChunkStorageChanges* storageChanges,
    uint32_t count,
    ChunkRef* chunks)
{
    for (uint32_t i = 0; i < count; i++) {
        ChunkRef* neighbours = storage->neighbours[chunks[i]];
        if (neighbours[CHUNK_NEGATIVE_X_NEIGHBOUR] != CHUNK_NEIGHBOUR_EMPTY) {
            storage->neighbours[neighbours[CHUNK_NEGATIVE_X_NEIGHBOUR]]
                               [CHUNK_POSITIVE_X_NEIGHBOUR]
                = CHUNK_NEIGHBOUR_EMPTY;
            ChunkStorageChanges_addNeighbourChanges(
                storageChanges,
                1,
                &neighbours[CHUNK_NEGATIVE_X_NEIGHBOUR]);
        }
        if (neighbours[CHUNK_NEGATIVE_Y_NEIGHBOUR] != CHUNK_NEIGHBOUR_EMPTY) {
            storage->neighbours[neighbours[CHUNK_NEGATIVE_Y_NEIGHBOUR]]
                               [CHUNK_POSITIVE_Y_NEIGHBOUR]
                = CHUNK_NEIGHBOUR_EMPTY;
            ChunkStorageChanges_addNeighbourChanges(
                storageChanges,
                1,
                &neighbours[CHUNK_NEGATIVE_Y_NEIGHBOUR]);
        }
        if (neighbours[CHUNK_NEGATIVE_Z_NEIGHBOUR] != CHUNK_NEIGHBOUR_EMPTY) {
            storage->neighbours[neighbours[CHUNK_NEGATIVE_Z_NEIGHBOUR]]
                               [CHUNK_POSITIVE_Z_NEIGHBOUR]
                = CHUNK_NEIGHBOUR_EMPTY;
            ChunkStorageChanges_addNeighbourChanges(
                storageChanges,
                1,
                &neighbours[CHUNK_NEGATIVE_Z_NEIGHBOUR]);
        }
        if (neighbours[CHUNK_POSITIVE_X_NEIGHBOUR] != CHUNK_NEIGHBOUR_EMPTY) {
            storage->neighbours[neighbours[CHUNK_POSITIVE_X_NEIGHBOUR]]
                               [CHUNK_NEGATIVE_X_NEIGHBOUR]
                = CHUNK_NEIGHBOUR_EMPTY;
            ChunkStorageChanges_addNeighbourChanges(
                storageChanges,
                1,
                &neighbours[CHUNK_POSITIVE_X_NEIGHBOUR]);
        }
        if (neighbours[CHUNK_POSITIVE_Y_NEIGHBOUR] != CHUNK_NEIGHBOUR_EMPTY) {
            storage->neighbours[neighbours[CHUNK_POSITIVE_Y_NEIGHBOUR]]
                               [CHUNK_NEGATIVE_Y_NEIGHBOUR]
                = CHUNK_NEIGHBOUR_EMPTY;
            ChunkStorageChanges_addNeighbourChanges(
                storageChanges,
                1,
                &neighbours[CHUNK_POSITIVE_Y_NEIGHBOUR]);
        }
        if (neighbours[CHUNK_POSITIVE_Z_NEIGHBOUR] != CHUNK_NEIGHBOUR_EMPTY) {
            storage->neighbours[neighbours[CHUNK_POSITIVE_Z_NEIGHBOUR]]
                               [CHUNK_NEGATIVE_Z_NEIGHBOUR]
                = CHUNK_NEIGHBOUR_EMPTY;
            ChunkStorageChanges_addNeighbourChanges(
                storageChanges,
                1,
                &neighbours[CHUNK_POSITIVE_Z_NEIGHBOUR]);
        }
    }
    IdAllocator_remove(&storage->idAllocator, count, chunks);
}

bool ChunkStorage_findChunkFromPos(
    ChunkStorage* storage,
    ivec3 pos,
    ChunkRef* chunk)
{
    if (!ChunkStorage_first(storage, chunk))
        return false;
    do {
        if (storage->positions[*chunk][0] == pos[0]
            && storage->positions[*chunk][1] == pos[1]
            && storage->positions[*chunk][2] == pos[2])
            return true;
    } while (ChunkStorage_next(storage, *chunk, chunk));
    return false;
}

inline bool ChunkStorage_first(
    ChunkStorage* storage,
    ChunkRef* chunk)
{
    return IdAllocator_first(&storage->idAllocator, chunk);
}

inline bool ChunkStorage_next(
    ChunkStorage* storage,
    ChunkRef lastChunk,
    ChunkRef* chunk)
{
    return IdAllocator_next(&storage->idAllocator, lastChunk, chunk);
}

void ChunkStorage_destroy(
    ChunkStorage* storage)
{
    IdAllocator_destroy(&storage->idAllocator);
    free(storage->colors);
    free(storage->neighbours);
    free(storage->positions);
}

/* CHUNK STORAGE CHANGES METHODS */

void ChunkStorageChanges_init(
    ChunkStorageChanges* storage,
    uint32_t voxBitMaskChangesCapacity,
    uint32_t colorChangesCapacity,
    uint32_t neighbourChangesCapacity)
{
    storage->voxBitMaskChangesCapacity = voxBitMaskChangesCapacity;
    storage->voxBitMaskChangesCount = 0;
    storage->voxBitMaskChangesDupesCheckedUntil = 0;
    storage->voxBitMaskChanges = (ChunkRef*)malloc(
        voxBitMaskChangesCapacity * sizeof(ChunkRef));
    storage->colorChangesCapacity = colorChangesCapacity;
    storage->colorChangesCount = 0;
    storage->colorChangesDupesCheckedUntil = 0;
    storage->colorChanges = (ChunkRef*)malloc(
        colorChangesCapacity * sizeof(ChunkRef));
    storage->neighbourChangesCapacity = neighbourChangesCapacity;
    storage->neighbourChangesCount = 0;
    storage->neighbourChangesDupesCheckedUntil = 0;
    storage->neighbourChanges = (ChunkRef*)malloc(
        neighbourChangesCapacity * sizeof(ChunkRef));
}

static void removeChunkDupes(
    uint32_t* count,
    uint32_t* dupesCheckedUntil,
    ChunkRef* chunks)
{
    for (
        uint32_t i = *dupesCheckedUntil;
        i < *count;
        i++) {
        bool dupe = false;
        for (uint32_t a = 0; a < *dupesCheckedUntil; a++) {
            if (chunks[i] == chunks[a]) {
                dupe = true;
                break;
            }
        }
        if (dupe == false) {
            chunks[*dupesCheckedUntil]
                = chunks[i];
            (*dupesCheckedUntil)++;
        }
    }
    *count = *dupesCheckedUntil;
}

void ChunkStorageChanges_addVoxBitMaskChanges(
    ChunkStorageChanges* storage,
    uint32_t chunksCount,
    ChunkRef* chunks)
{
    if (storage->voxBitMaskChangesCount + chunksCount > storage->voxBitMaskChangesCapacity) {
        removeChunkDupes(
            &storage->voxBitMaskChangesCount,
            &storage->voxBitMaskChangesDupesCheckedUntil,
            storage->voxBitMaskChanges);
        if (storage->voxBitMaskChangesCount + chunksCount > storage->voxBitMaskChangesCapacity) {
            storage->voxBitMaskChangesCapacity
                = storage->voxBitMaskChangesCount + chunksCount + 10;
            storage->voxBitMaskChanges = (ChunkRef*)realloc(
                storage->voxBitMaskChanges,
                storage->voxBitMaskChangesCapacity);
        }
    }
    for (int i = 0; i < chunksCount; i++)
        storage->voxBitMaskChanges[storage->voxBitMaskChangesCount + i] = chunks[i];
    storage->voxBitMaskChangesCount += chunksCount;
}

void ChunkStorageChanges_addColorChanges(
    ChunkStorageChanges* storage,
    uint32_t chunksCount,
    ChunkRef* chunks)
{
    if (storage->colorChangesCount + chunksCount > storage->colorChangesCapacity) {
        removeChunkDupes(
            &storage->colorChangesCount,
            &storage->colorChangesDupesCheckedUntil,
            storage->colorChanges);
        if (storage->colorChangesCount + chunksCount > storage->colorChangesCapacity) {
            storage->colorChangesCapacity
                = storage->colorChangesCount + chunksCount + 10;
            storage->colorChanges = (ChunkRef*)realloc(
                storage->colorChanges,
                storage->colorChangesCapacity);
        }
    }
    for (int i = 0; i < chunksCount; i++)
        storage->colorChanges[storage->colorChangesCount + i] = chunks[i];
    storage->colorChangesCount += chunksCount;
}

void ChunkStorageChanges_addNeighbourChanges(
    ChunkStorageChanges* storage,
    uint32_t chunksCount,
    ChunkRef* chunks)
{
    if (storage->neighbourChangesCount + chunksCount > storage->neighbourChangesCapacity) {
        removeChunkDupes(
            &storage->neighbourChangesCount,
            &storage->neighbourChangesDupesCheckedUntil,
            storage->neighbourChanges);
        if (storage->neighbourChangesCount + chunksCount > storage->neighbourChangesCapacity) {
            storage->neighbourChangesCapacity
                = storage->neighbourChangesCount + chunksCount + 50;
            storage->neighbourChanges = (ChunkRef*)realloc(
                storage->neighbourChanges,
                storage->neighbourChangesCapacity);
        }
    }
    for (int i = 0; i < chunksCount; i++)
        storage->neighbourChanges[storage->neighbourChangesCount + i] = chunks[i];
    storage->neighbourChangesCount += chunksCount;
}

void ChunkStorageChanges_destroy(ChunkStorageChanges* storage)
{
    free(storage->colorChanges);
    free(storage->neighbourChanges);
}

/* CHUNK GPU STORAGE METHODS */

void ChunkGpuStorage_init(
    ChunkGpuStorage* storage,
    VkDevice logicalDevice,
    VkPhysicalDevice physicalDevice)
{
    createBuffer(
        logicalDevice,
        physicalDevice,
        CHUNK_CAPACITY * sizeof(uint8_t[CHUNK_BIT_MASK_SIZE]),
        0,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &storage->voxBitMask,
        &storage->voxBitMaskMemory);
    createBuffer(
        logicalDevice,
        physicalDevice,
        CHUNK_CAPACITY * sizeof(uint8_t[CHUNK_VOX_COUNT]),
        0,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &storage->colors,
        &storage->colorsMemory);
    createBuffer(
        logicalDevice,
        physicalDevice,
        CHUNK_CAPACITY * sizeof(uint32_t[CHUNK_VOX_COUNT]),
        0,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &storage->brightness,
        &storage->brightnessMemory);
    createBuffer(
        logicalDevice,
        physicalDevice,
        CHUNK_CAPACITY * sizeof(ChunkRef[6]),
        0,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &storage->neighbours,
        &storage->neighboursMemory);
}

void ChunkGpuStorage_update(
    ChunkGpuStorage* gpuStorage,
    VkDevice logicalDevice,
    ChunkStorage* cpuStorage,
    ChunkStorageChanges* changes)
{
    removeChunkDupes(
        &changes->voxBitMaskChangesCount,
        &changes->voxBitMaskChangesDupesCheckedUntil,
        changes->voxBitMaskChanges);
    for (uint32_t i = 0; i < changes->voxBitMaskChangesCount; i++) {
        copyDataToBuffer(
            logicalDevice,
            cpuStorage->voxBitMask[changes->voxBitMaskChanges[i]],
            gpuStorage->voxBitMaskMemory,
            changes->voxBitMaskChanges[i] * sizeof(uint8_t[CHUNK_BIT_MASK_SIZE]),
            sizeof(uint8_t[CHUNK_BIT_MASK_SIZE]));
    }
    removeChunkDupes(
        &changes->colorChangesCount,
        &changes->colorChangesDupesCheckedUntil,
        changes->colorChanges);
    for (uint32_t i = 0; i < changes->colorChangesCount; i++) {
        copyDataToBuffer(
            logicalDevice,
            cpuStorage->colors[changes->colorChanges[i]],
            gpuStorage->colorsMemory,
            changes->colorChanges[i] * sizeof(uint8_t[CHUNK_VOX_COUNT]),
            sizeof(uint8_t[CHUNK_VOX_COUNT]));
    }
    removeChunkDupes(
        &changes->neighbourChangesCount,
        &changes->neighbourChangesDupesCheckedUntil,
        changes->neighbourChanges);
    for (uint32_t i = 0; i < changes->neighbourChangesCount; i++) {
        copyDataToBuffer(
            logicalDevice,
            cpuStorage->neighbours[changes->neighbourChanges[i]],
            gpuStorage->neighboursMemory,
            changes->neighbourChanges[i] * sizeof(ChunkRef[6]),
            sizeof(ChunkRef[6]));
    }
}

void ChunkGpuStorage_destroy(
    ChunkGpuStorage* storage,
    VkDevice logicalDevice)
{
    vkDestroyBuffer(logicalDevice, storage->voxBitMask, NULL);
    vkFreeMemory(logicalDevice, storage->voxBitMaskMemory, NULL);
    vkDestroyBuffer(logicalDevice, storage->colors, NULL);
    vkFreeMemory(logicalDevice, storage->colorsMemory, NULL);
    vkDestroyBuffer(logicalDevice, storage->brightness, NULL);
    vkFreeMemory(logicalDevice, storage->brightnessMemory, NULL);
    vkDestroyBuffer(logicalDevice, storage->neighbours, NULL);
    vkFreeMemory(logicalDevice, storage->neighboursMemory, NULL);
}
