#include "shader_module.h"

#include <stdio.h>
#include <stdlib.h>

#include "vk_utils/exceptions.h"

static void readFile(const char *fileName, long *fileLength, char **fileBytes)
{
    FILE *file = fopen(fileName, "rb");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        *fileLength = ftell(file);
        fseek(file, 0, SEEK_SET);

        *fileBytes = (char *)malloc(*fileLength);
        fread(*fileBytes, 1, *fileLength, file);

        fclose(file);
    }
    else
    {
        printf("Exiting because file '%s' could not be opened\n", fileName);
        exit(EXIT_FAILURE);
    }
}

void createShaderModule(
    VkDevice device,
    const char* srcFileName,
    VkShaderModule* shaderModule)
{
    long spvLength;
    char *spv;
    readFile(srcFileName, &spvLength, &spv);

    VkShaderModuleCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.codeSize = spvLength;
    createInfo.pCode = (uint32_t *)spv;

    handleVkResult(
        vkCreateShaderModule(device, &createInfo, NULL, shaderModule),
        "creating shader module");
    free(spv);
}
