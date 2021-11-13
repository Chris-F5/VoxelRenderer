#ifndef DEBUG_LINES_H
#define DEBUG_LINES_H

#include <vulkan/vulkan.h>

#include <cglm/types.h>

extern const VkVertexInputBindingDescription DEBUG_LINE_VERTEX_BINDING_DESCRIPTIONS[];
extern const size_t DEBUG_LINE_VERTEX_BINDING_DESCRIPTION_COUNT;

extern const VkVertexInputAttributeDescription DEBUG_LINE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[];
extern const size_t DEBUG_LINE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT;

typedef struct {
    vec3 pos;
    vec3 color;
} DebugLineVertex;

typedef struct {
    uint32_t vertCapacity;
    uint32_t vertCount;

    VkBuffer vertBuffer;
    VkDeviceMemory vertBufferMemory;
} DebugLineStorage;

void DebugLineStorage_init(
    DebugLineStorage* storage,
    VkDevice logicalDevice,
    VkPhysicalDevice physicalDevice,
    uint32_t vertCapacity);

void DebugLineStorage_update(
    DebugLineStorage* storage,
    VkDevice logicalDevice,
    uint32_t vertCount,
    const DebugLineVertex* verts);

void DebugLineStorage_destroy(
    DebugLineStorage* storage,
    VkDevice logicalDevice);

#endif
