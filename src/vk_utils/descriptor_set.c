#include "./descriptor_set.h"

#include <stdlib.h>

#include "./exceptions.h"

void allocateDescriptorSets(
    VkDevice device,
    VkDescriptorSetLayout layout,
    VkDescriptorPool descriptorPool,
    uint32_t count,
    VkDescriptorSet* descriptorSets)
{
    VkDescriptorSetLayout* setLayoutCopies
        = (VkDescriptorSetLayout*)malloc(count * sizeof(VkDescriptorSetLayout));
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
