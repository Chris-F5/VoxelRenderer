#ifndef POINTMAP_OBJECT_LOADER_H
#define POINTMAP_OBJECT_LOADER_H

#include "./vox_object.h"
#include <stdio.h>

void loadVoxObjectFromPointmapFile(
    VkDevice logicalDevice,
    VoxBlockStorage* blockStorage,
    VoxPaletteStorage* paletteStorage,
    ModelStorage* modelStorage,
    vec3 pos,
    FILE* file,
    VoxObject* object);

#endif
