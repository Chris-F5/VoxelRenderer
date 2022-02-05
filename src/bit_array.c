#include "bit_array.h"

#include <stdlib.h>

void setBit(uint8_t* bitArray, uint32_t index)
{
    bitArray[index / 8] |= 1 << (index % 8);
}

void unsetBit(uint8_t* bitArray, uint32_t index)
{
    bitArray[index / 8] &= ~(1 << (index % 8));
}

uint8_t testBit(uint8_t* bitArray, uint32_t index)
{
    uint8_t t = bitArray[index / 8] & (1 << (index % 8));
    return t;
}
