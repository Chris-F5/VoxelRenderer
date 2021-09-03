#include "descriptor_set.h"

#include <stdlib.h>

#include "buffer.h"
#include "exceptions.h"

void allocateDescriptorSets(
    VkDevice device,
    VkDescriptorSetLayout layout,
    VkDescriptorPool descriptorPool,
    uint32_t count,
    VkDescriptorSet* descriptorSets)
{
    VkDescriptorSetLayout* setLayoutCopies = malloc(count * sizeof(VkDescriptorSetLayout));
    for (int i = 0; i < count; i++)
        setLayoutCopies[i] = layout;

    VkDescriptorSetAllocateInfo setAllocInfo;
    setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocInfo.pNext = NULL;
    setAllocInfo.descriptorPool = descriptorPool;
    setAllocInfo.descriptorSetCount = count;
    setAllocInfo.pSetLayouts = setLayoutCopies;

    handleVkResult(
        vkAllocateDescriptorSets(device, &setAllocInfo, descriptorSets),
        "allocating descriptor sets");

    free(setLayoutCopies);
}

void createUniformBuffers(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    size_t uniformBufferSize,
    uint32_t count,
    VkBuffer* buffers,
    VkDeviceMemory* buffersMemory)
{
    for (int i = 0; i < count; i++)
        createBuffer(
            device,
            physicalDevice,
            uniformBufferSize,
            0,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            &buffers[i],
            &buffersMemory[i]);
}
