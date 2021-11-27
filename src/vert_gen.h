#ifndef VERT_GEN_H
#define VERT_GEN_H

#include <vulkan/vulkan.h>

#include "./models.h"

#define MAX_CHUNK_VERT_COUNT (CHUNK_VOX_COUNT * 18)

void generateChunkVertices(
    VkDevice logicalDevice,
    const vec3* colorPalette,
    const unsigned char* chunkColors,
    ModelStorage* modelStorage,
    ModelRef targetModel);

#endif
