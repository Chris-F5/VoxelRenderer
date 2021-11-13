#ifndef VERT_GEN_H
#define VERT_GEN_H

#include <vulkan/vulkan.h>

#include "./models.h"

void generateVoxBlockVertices(
    VkDevice logicalDevice,
    const vec3* colorPalette,
    const unsigned char* blockColors,
    ModelStorage* modelStorage,
    ModelRef targetModel);

#endif
