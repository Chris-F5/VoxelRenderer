#ifndef OBJECT
#define OBJECT

#include <stdbool.h>

#include <vulkan/vulkan.h>

#include "renderer/scene_data/scene_data.h"

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    BlockRef* blocks;
    bool* blocksMask;
} Object;

Object createObject(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    SceneData* sceneData,
    FILE* objectFile);

void cleanupObject(SceneData* sceneData, Object object);

#endif
