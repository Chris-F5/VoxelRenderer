#ifndef OBJECT
#define OBJECT

#include <stdbool.h>

#include <vulkan/vulkan.h>

#include "renderer/scene_data/scene_data.h"

typedef struct {
    vec3 pos;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    PaletteRef palette;
    BlockRef* blocks;
    bool* blocksMask;
} Object;

Object createEmptyObject(
    SceneData* sceneData,
    vec3 pos,
    uint32_t width,
    uint32_t height,
    uint32_t depth,
    PaletteRef palette);

Object createObjectFromFile(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    SceneData* sceneData,
    vec3 pos,
    FILE* objectFile);

void setObjectVoxel(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    SceneData* sceneData,
    Object* object,
    ivec3 pos,
    Voxel vox);

void updateObjectVertexBuffers(
    VkDevice device,
    SceneData* sceneData,
    Object object);

void cleanupObject(SceneData* sceneData, Object object);

#endif
