#ifndef SHADER_MODULE
#define SHADER_MODULE

#include <vulkan/vulkan.h>

VkShaderModule createShaderModule(VkDevice device, const char *srcFileName);

#endif
