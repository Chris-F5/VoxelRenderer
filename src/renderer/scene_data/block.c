#include "block.h"

#include "../vk_utils/buffer.h"

const uint32_t INDEX_BUFFER[] = {
    0, 1, 2,
    1, 3, 2,
    4, 1, 0,
    4, 5, 1
};

void createIndexBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t* indexCount,
    VkBuffer* indexBuffer,
    VkDeviceMemory* indexBufferMemory)
{
    *indexCount = sizeof(INDEX_BUFFER) / sizeof(INDEX_BUFFER[0]);

    createBuffer(
        device,
        physicalDevice,
        sizeof(INDEX_BUFFER),
        0,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        indexBuffer,
        indexBufferMemory);

    copyDataToBuffer(
        device,
        (void*)INDEX_BUFFER,
        *indexBufferMemory,
        0,
        sizeof(INDEX_BUFFER));
}
