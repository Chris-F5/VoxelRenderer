#include "pointmap_object_loader.h"

#include <limits.h>
#include <stdlib.h>

char END_HEADER[] = "end_header\r\n";

void skipPlyHeader(FILE* file)
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
Object loadPointmapObjectFile(VkDevice device,
    VkPhysicalDevice physicalDevice,
    SceneData* sceneData,
    vec3 pos,
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

    PaletteRef palette = createEmptyPalette(sceneData);

    Object object = createEmptyObject(
        sceneData,
        pos,
        (maxX - minX + 1) / VOX_BLOCK_SCALE + 1,
        (maxY - minY + 1) / VOX_BLOCK_SCALE + 1,
        (maxZ - minZ + 1) / VOX_BLOCK_SCALE + 1,
        palette);

    vec3* paletteData = getPalette(sceneData, palette);
    unsigned int paletteColors = 1;

    fseek(file, endOfHeader, SEEK_SET);

    while (fscanf(file, "%d %d %d %d %d %d\n", &x, &y, &z, &r, &g, &b) != EOF) {
        ivec3 voxPos;
        voxPos[0] = x - minX;
        voxPos[1] = y - minY;
        voxPos[2] = z - minZ;
        unsigned char voxColorId = 1;
        while (paletteData[voxColorId][0] != (float)r / 255
            || paletteData[voxColorId][1] != (float)g / 255
            || paletteData[voxColorId][2] != (float)b / 255) {
            if (voxColorId == UCHAR_MAX) {
                puts("Pointmap object file colors do not fit in palette. Exiting.");
                exit(EXIT_FAILURE);
            }
            if (voxColorId == paletteColors) {
                paletteData[voxColorId][0] = (float)r / 255;
                paletteData[voxColorId][1] = (float)g / 255;
                paletteData[voxColorId][2] = (float)b / 255;
                paletteColors++;
                break;
            }
            voxColorId++;
        }
        setObjectVoxel(device, physicalDevice, sceneData, &object, voxPos, voxColorId);
    }

    updateObjectVertexBuffers(
        device,
        sceneData,
        object);
    return object;
}