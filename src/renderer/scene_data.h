#ifndef SCENE_DATA
#define SCENE_DATA

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cglm/types.h>

typedef struct
{
    vec2 pos;
    vec3 color;
} Vertex;

extern const Vertex VERTICES[];
extern const size_t VERTEX_COUNT;

extern const VkVertexInputBindingDescription VERTEX_BINDING_DESCRIPTIONS[];
extern const size_t VERTEX_BINDING_DESCRIPTION_COUNT;

extern const VkVertexInputAttributeDescription VERTEX_INPUT_ATTRIBUTE_DESCRIPTIONS[];
extern const size_t VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_COUNT;

void createVertexBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkQueue queue,
    VkCommandPool commandPool,
    VkBuffer* stagingBuffer,
    VkDeviceMemory* stagingBufferMemory,
    VkBuffer* vertexBuffer,
    VkDeviceMemory* vertexBufferMemory);

#endif
