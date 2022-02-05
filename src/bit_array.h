#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

#include <stdint.h>

void setBit(uint8_t* bitArray, uint32_t index);

void unsetBit(uint8_t* bitArray, uint32_t index);

uint8_t testBit(uint8_t* bitArray, uint32_t index);

#endif
