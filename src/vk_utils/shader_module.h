#ifndef SHADER_MODULE_H
#define SHADER_MODULE_H

#include <vulkan/vulkan.h>

void createShaderModule(
    VkDevice device,
    const char* srcFileName,
    VkShaderModule* shaderModule);

#endif
