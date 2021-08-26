#ifndef VERTEX_GRID
#define VERTEX_GRID

#include <vulkan/vulkan.h>
#include <cglm/types.h>

extern const VkVertexInputBindingDescription VERTEX_BINDING_DESCRIPTIONS[];
extern const size_t VERTEX_BINDING_DESCRIPTION_COUNT;

extern const VkVertexInputAttributeDescription VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[];
extern const size_t VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT;

typedef struct {
    vec3 pos;
} Vertex;

void createVertexGrid(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkQueue queue,
    VkCommandPool commandPool,
    uint32_t gridSize,
    uint32_t* vertexCount,
    VkBuffer* vertexBuffer,
    VkDeviceMemory* vertexBufferMemory);

#endif
