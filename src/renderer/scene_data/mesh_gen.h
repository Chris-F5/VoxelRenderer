#ifndef MESH_GEN
#define MESH_GEN

#include <cglm/types.h>
#include <vulkan/vulkan.h>

#include "scene_data.h"

extern const VkVertexInputBindingDescription VERTEX_BINDING_DESCRIPTIONS[];
extern const size_t VERTEX_BINDING_DESCRIPTION_COUNT;

extern const VkVertexInputAttributeDescription VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[];
extern const size_t VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT;

void createEmptyBlockVertexBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t* vertexBufferLength,
    VkBuffer* vertexBuffer,
    VkDeviceMemory* vertexBufferMemory);

void writeBlockVertexBuffer(
    VkDevice device,
    const Voxel* voxels,
    const vec3* palette,
    uint32_t* vertexBufferLength,
    VkBuffer* vertexBuffer,
    VkDeviceMemory* vertexBufferMemory);

#endif
