#include "command_buffer.h"

#include "exceptions.h"

VkCommandPool createCommandPool(
    VkDevice device,
    VkCommandPoolCreateFlags flags,
    uint32_t queueFamily)
{
    VkCommandPoolCreateInfo poolCreateInfo;
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreateInfo.pNext = NULL;
    poolCreateInfo.flags = flags;
    poolCreateInfo.queueFamilyIndex = queueFamily;

    VkCommandPool commandPool;
    handleVkResult(
        vkCreateCommandPool(device, &poolCreateInfo, NULL, &commandPool),
        "creating command pool");

    return commandPool;
}

void allocateCommandBuffers(
    VkDevice device,
    VkCommandPool commandPool,
    size_t count,
    VkCommandBuffer *commandBuffers)
{
    VkCommandBufferAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    handleVkResult(
        vkAllocateCommandBuffers(device, &allocInfo, commandBuffers),
        "allocating command buffers");
}
