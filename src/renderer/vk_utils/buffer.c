#include "buffer.h"

#include <string.h>

#include "exceptions.h"
#include "physical_device_memory.h"
#include "command_buffer.h"

void createBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkDeviceSize size,
    VkBufferCreateFlags flags,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkBuffer* buffer,
    VkDeviceMemory* bufferMemory)
{
    VkBufferCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = flags;
    createInfo.size = size;
    createInfo.usage = usageFlags;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = NULL;

    handleVkResult(
        vkCreateBuffer(device, &createInfo, NULL, buffer),
        "creating buffer");

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(device, *buffer, &memReq);

    VkMemoryAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memReq.memoryTypeBits, memoryPropertyFlags);

    handleVkResult(
        vkAllocateMemory(device, &allocInfo, NULL, bufferMemory),
        "allocating buffer");

    vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
}

void copyDataToBuffer(
    VkDevice device,
    const void* data,
    VkDeviceMemory bufferMemory,
    size_t offset,
    size_t size)
{
    void* dst;
    vkMapMemory(device, bufferMemory, offset, size, 0, &dst);
    memcpy(dst, data, size);
    vkUnmapMemory(device, bufferMemory);
}

void bufferTransfer(
    VkDevice device,
    VkQueue queue,
    VkCommandPool commandPool,
    uint32_t copyRegionCount,
    VkBufferCopy *copyRegions,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer)
{
    VkCommandBuffer commandBuffer;
    allocateCommandBuffers(
        device,
        commandPool,
        1,
        &commandBuffer
    );

    VkCommandBufferBeginInfo beginInfo;
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = NULL;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = NULL;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, copyRegionCount, copyRegions);

    handleVkResult(
        vkEndCommandBuffer(commandBuffer),
        "recording buffer transfer command");

    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = NULL;
    submitInfo.pWaitDstStageMask = NULL;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = NULL;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);

    vkQueueWaitIdle(queue);
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}    
