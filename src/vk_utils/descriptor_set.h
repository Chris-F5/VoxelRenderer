#ifndef DESCRIPTOR_SET_H
#define DESCRIPTOR_SET_H

#include <vulkan/vulkan.h>

void allocateDescriptorSets(
    VkDevice device,
    VkDescriptorSetLayout layout,
    VkDescriptorPool descriptorPool,
    uint32_t count,
    VkDescriptorSet* descriptorSets);

#endif
