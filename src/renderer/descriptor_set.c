#include "descriptor_set.h"

#include <stdlib.h>
#include <string.h>

#include "vk_utils/buffer.h"
#include "vk_utils/exceptions.h"

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

void createDescriptorSetLayouts(
    VkDevice device,
    VkDescriptorSetLayout* globalDescriptorSetLayout,
    VkDescriptorSetLayout* meshDescriptorSetLayout)
{
    // GLOBAL SET LAYOUT
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

        handleVkResult(
            vkCreateDescriptorSetLayout(device, &layoutCreateInfo, NULL, globalDescriptorSetLayout),
            "creating global descriptor set layout");
    }
    // MESH SET LAYOUT
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

        handleVkResult(
            vkCreateDescriptorSetLayout(device, &layoutCreateInfo, NULL, meshDescriptorSetLayout),
            "creating mesh descriptor set layout");
    }
}

void createGlobalDescriptorSets(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkDescriptorSetLayout layout,
    uint32_t count,
    VkDescriptorPool* descriptorPool,
    VkDescriptorSet* descriptorSets,
    VkBuffer* uniformBuffers,
    VkDeviceMemory* uniformBuffersMemory)
{
    // DESCRIPTOR POOL

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

    // ALLOCATE

    allocateDescriptorSets(
        device,
        layout,
        *descriptorPool,
        count,
        descriptorSets);

    // CREATE BUFFERS

    createUniformBuffers(
        device,
        physicalDevice,
        sizeof(GlobalUniformBuffer),
        count,
        uniformBuffers,
        uniformBuffersMemory);

    // BIND BUFFERS TO DESCRIPTOR SETS

    for (int i = 0; i < count; i++) {
        VkDescriptorBufferInfo uniformBufferInfo;
        uniformBufferInfo.buffer = uniformBuffers[i];
        uniformBufferInfo.offset = 0;
        uniformBufferInfo.range = sizeof(GlobalUniformBuffer);

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

void createMeshDescriptorPool(
    VkDevice device,
    uint32_t maxMeshCount,
    VkDescriptorPool* descriptorPool)
{
    VkDescriptorPoolSize poolSize;
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = maxMeshCount;

    VkDescriptorPoolCreateInfo poolCreateInfo;
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.pNext = NULL;
    poolCreateInfo.flags = 0;
    poolCreateInfo.maxSets = maxMeshCount;
    poolCreateInfo.poolSizeCount = 1;
    poolCreateInfo.pPoolSizes = &poolSize;

    handleVkResult(
        vkCreateDescriptorPool(device, &poolCreateInfo, NULL, descriptorPool),
        "creating descriptor pool");
}

void createMeshDescriptorSets(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkDescriptorSetLayout layout,
    uint32_t count,
    VkDescriptorPool descriptorPool,
    VkDescriptorSet* descriptorSets,
    VkBuffer* uniformBuffers,
    VkDeviceMemory* uniformBuffersMemory)
{
    // ALLOCATE

    allocateDescriptorSets(
        device,
        layout,
        descriptorPool,
        count,
        descriptorSets);

    // CREATE BUFFERS

    createUniformBuffers(
        device,
        physicalDevice,
        sizeof(MeshUniformBuffer),
        count,
        uniformBuffers,
        uniformBuffersMemory);

    // BIND BUFFERS TO DESCRIPTOR SETS

    for (int i = 0; i < count; i++) {
        VkDescriptorBufferInfo uniformBufferInfo;
        uniformBufferInfo.buffer = uniformBuffers[i];
        uniformBufferInfo.offset = 0;
        uniformBufferInfo.range = sizeof(MeshUniformBuffer);

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
