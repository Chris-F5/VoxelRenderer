#include "./pointmap_object_loader.h"

#include <limits.h>
#include <math.h>
#include <stdlib.h>

char END_HEADER[] = "end_header\r\n";

static void skipPlyHeader(FILE* file)
{
    int endHeaderIndex = 0;
    int c;
    while (endHeaderIndex < sizeof(END_HEADER) - 1) {
        c = fgetc(file);
        if (c == EOF) {
            puts("Pointmap object file does not end header. Exiting.");
            exit(EXIT_FAILURE);
        }
        if (c == END_HEADER[endHeaderIndex])
            endHeaderIndex++;
        else
            endHeaderIndex = 0;
    }
}

void loadChunksFromPointmapFile(
    ChunkStorage* chunkStorage,
    ChunkStorageChanges* chunkStorageChanges,
    VoxPaletteStorage* paletteStorage,
    VoxPaletteRef palette,
    FILE* file)
{
    if (file == NULL) {
        puts("Pointmap object file is NULL. Exiting.");
        exit(EXIT_FAILURE);
    }
    skipPlyHeader(file);
    long int endOfHeader = ftell(file);

    int minX, minY, minZ;
    int maxX, maxY, maxZ;
    minX = minY = minZ = INT_MAX;
    maxX = maxY = maxZ = INT_MIN;

    int x, y, z, r, g, b = 0;
    while (fscanf(file, "%d %d %d %d %d %d\n", &x, &y, &z, &r, &g, &b) != EOF) {
        if (x < minX)
            minX = x;
        if (y < minY)
            minY = y;
        if (z < minZ)
            minZ = z;
        if (x > maxX)
            maxX = x;
        if (y > maxY)
            maxY = y;
        if (z > maxZ)
            maxZ = z;
    }

    vec3* paletteColorData
        = VoxPaletteStorage_getColorData(paletteStorage, palette);
    unsigned int paletteColors = 1;

    fseek(file, endOfHeader, SEEK_SET);

    while (fscanf(file, "%d %d %d %d %d %d\n", &x, &y, &z, &r, &g, &b) != EOF) {
        ivec3 voxPos;
        voxPos[0] = x - minX;
        voxPos[1] = y - minY;
        voxPos[2] = z - minZ;
        unsigned char voxColorId = 1;
        while (paletteColorData[voxColorId][0] != (float)r / 255
            || paletteColorData[voxColorId][1] != (float)g / 255
            || paletteColorData[voxColorId][2] != (float)b / 255) {
            if (voxColorId == 255) {
                puts("Pointmap object file colors do not fit in palette. Exiting.");
                exit(EXIT_FAILURE);
            }
            if (voxColorId == paletteColors) {
                paletteColorData[voxColorId][0] = (float)r / 255;
                paletteColorData[voxColorId][1] = (float)g / 255;
                paletteColorData[voxColorId][2] = (float)b / 255;
                paletteColors++;
                break;
            }
            voxColorId++;
        }

        ivec3 chunkPos;
        chunkPos[0] = (int)floorf(voxPos[0] / (float)CHUNK_SCALE);
        chunkPos[1] = (int)floorf(voxPos[1] / (float)CHUNK_SCALE);
        chunkPos[2] = (int)floorf(voxPos[2] / (float)CHUNK_SCALE);
        ChunkRef chunk;
        if (!ChunkStorage_findChunkFromPos(chunkStorage, chunkPos, &chunk)) {
            ChunkStorage_add(
                chunkStorage,
                chunkStorageChanges,
                1,
                &chunkPos,
                &chunk);
        }
        uint32_t voxId
            = voxPos[0] % CHUNK_SCALE
            + voxPos[1] % CHUNK_SCALE * CHUNK_SCALE
            + voxPos[2] % CHUNK_SCALE * CHUNK_SCALE * CHUNK_SCALE;
        ChunkStorage_chunkColorData(chunkStorage, chunk)[voxId] = voxColorId;
    }
}
