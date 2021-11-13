#include "./buffer.h"

#include <stdio.h>
#include <string.h>

#include "./exceptions.h"
#include "./physical_device_memory.h"

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

    handleVkResult(
        vkBindBufferMemory(device, *buffer, *bufferMemory, 0),
        "binding buffer meomry");
}

void copyDataToBuffer(
    VkDevice device,
    const void* data,
    VkDeviceMemory bufferMemory,
    size_t offset,
    size_t size)
{
    void* dst;
    handleVkResult(
        vkMapMemory(device, bufferMemory, offset, size, 0, &dst),
        "mapping buffer memory for copy");
    memcpy(dst, data, size);
    vkUnmapMemory(device, bufferMemory);
}
