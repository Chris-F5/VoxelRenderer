#ifndef POINTMAP_OBJECT_LOADER
#define POINTMAP_OBJECT_LOADER

#include <vulkan/vulkan.h>

#include "object.h"

Object loadPointmapObjectFile(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    SceneData* sceneData,
    vec3 pos,
    FILE* file);

#endif
