#ifndef DEBUG_LINES
#define DEBUG_LINES

#include <cglm/types.h>
#include <vulkan/vulkan.h>

extern const VkVertexInputBindingDescription DEBUG_LINE_VERTEX_BINDING_DESCRIPTIONS[];
extern const size_t DEBUG_LINE_VERTEX_BINDING_DESCRIPTION_COUNT;

extern const VkVertexInputAttributeDescription DEBUG_LINE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[];
extern const size_t DEBUG_LINE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT;

typedef struct {
    vec3 pos;
    vec3 color;
} DebugLineVertex;

typedef struct {
    uint32_t capacity;
    uint32_t vertCount;
    DebugLineVertex* verts;
    VkBuffer vertBuffer;
    VkDeviceMemory vertBufferMemory;
} DebugLineData;

DebugLineData createEmptyDebugLineData(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t initialCapacity);

void updateDebugLineData(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t newVertsLength,
    const DebugLineVertex* newVerts,
    DebugLineData* debugLineData);

void cleanupDebugLineData(
    VkDevice device,
    DebugLineData debugLineData);

#endif
