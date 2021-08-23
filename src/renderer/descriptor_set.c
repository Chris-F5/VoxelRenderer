#include "descriptor_set.h"

#include <stdlib.h>
#include <string.h>

#include "vk_utils/buffer.h"
#include "vk_utils/exceptions.h"

void createUniformBuffers(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t count,
    VkBuffer* buffers,
    VkDeviceMemory* buffersMemory)
{
    for (int i = 0; i < count; i++)
        createBuffer(
            device,
            physicalDevice,
            sizeof(UniformBuffer),
            0,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            &buffers[i],
            &buffersMemory[i]);
}

VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device)
{
    VkDescriptorSetLayoutBinding uboBinding;
    uboBinding.binding = 0;
    uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboBinding.descriptorCount = 1;
    uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboBinding.pImmutableSamplers = NULL;

    VkDescriptorSetLayoutCreateInfo layoutCreateInfo;
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreateInfo.pNext = NULL;
    layoutCreateInfo.flags = 0;
    layoutCreateInfo.bindingCount = 1;
    layoutCreateInfo.pBindings = &uboBinding;

    VkDescriptorSetLayout descriptorSetLayout;
    handleVkResult(
        vkCreateDescriptorSetLayout(device, &layoutCreateInfo, NULL, &descriptorSetLayout),
        "creating uniform buffer descriptor set layout");

    return descriptorSetLayout;
}

void createDescriptorSets(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    uint32_t count,
    VkDescriptorPool* descriptorPool,
    VkDescriptorSetLayout* descriptorSetLayout,
    VkDescriptorSet* descriptorSets,
    VkBuffer* uniformBuffers,
    VkDeviceMemory* uniformBuffersMemory)
{
    VkDescriptorPoolSize poolSize;
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = count;

    VkDescriptorPoolCreateInfo poolCreateInfo;
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.pNext = NULL;
    poolCreateInfo.flags = 0;
    poolCreateInfo.maxSets = count;
    poolCreateInfo.poolSizeCount = 1;
    poolCreateInfo.pPoolSizes = &poolSize;

    handleVkResult(
        vkCreateDescriptorPool(device, &poolCreateInfo, NULL, descriptorPool),
        "creating descriptor pool");

    *descriptorSetLayout = createDescriptorSetLayout(device);
    VkDescriptorSetLayout* setLayoutCopies = malloc(count * sizeof(VkDescriptorSetLayout));
    for (int i = 0; i < count; i++)
        setLayoutCopies[i] = *descriptorSetLayout;

    VkDescriptorSetAllocateInfo setAllocInfo;
    setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocInfo.pNext = NULL;
    setAllocInfo.descriptorPool = *descriptorPool;
    setAllocInfo.descriptorSetCount = count;
    setAllocInfo.pSetLayouts = setLayoutCopies;

    handleVkResult(
        vkAllocateDescriptorSets(device, &setAllocInfo, descriptorSets),
        "allocating descriptor sets");

    free(setLayoutCopies);

    createUniformBuffers(
        device,
        physicalDevice,
        count,
        uniformBuffers,
        uniformBuffersMemory);

    for (int i = 0; i < count; i++) {
        VkDescriptorBufferInfo uniformBufferInfo;
        uniformBufferInfo.buffer = uniformBuffers[i];
        uniformBufferInfo.offset = 0;
        uniformBufferInfo.range = sizeof(UniformBuffer);

        VkWriteDescriptorSet uniformBufferDescriptorWrite;
        uniformBufferDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        uniformBufferDescriptorWrite.pNext = NULL;
        uniformBufferDescriptorWrite.dstSet = descriptorSets[i];
        uniformBufferDescriptorWrite.dstBinding = 0;
        uniformBufferDescriptorWrite.dstArrayElement = 0;
        uniformBufferDescriptorWrite.descriptorCount = 1;
        uniformBufferDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferDescriptorWrite.pImageInfo = NULL;
        uniformBufferDescriptorWrite.pBufferInfo = &uniformBufferInfo;
        uniformBufferDescriptorWrite.pTexelBufferView = NULL;

        vkUpdateDescriptorSets(device, 1, &uniformBufferDescriptorWrite, 0, NULL);
    }
}
