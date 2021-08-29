#ifndef BLOCK
#define BLOCK

#include <vulkan/vulkan.h>
#include <cglm/types.h>

extern const VkVertexInputBindingDescription VERTEX_BINDING_DESCRIPTIONS[];
extern const size_t VERTEX_BINDING_DESCRIPTION_COUNT;

extern const VkVertexInputAttributeDescription VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[];
extern const size_t VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT;

extern const uint32_t VOX_BLOCK_SCALE;
extern const uint32_t VOX_BLOCK_VOX_COUNT;

typedef struct {
    vec3 pos;
    vec3 color;
} Vertex;

typedef struct
{
    vec3 color;
}Voxel;

void createBlockVertices(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkQueue queue,
    VkCommandPool commandPool,
    const Voxel* block,
    uint32_t* vertexCount,
    VkBuffer* vertexBuffer,
    VkDeviceMemory* vertexBufferMemory,
    uint32_t* indexCount,
    VkBuffer* indexBuffer,
    VkDeviceMemory* indexBufferMemory);

#endif
