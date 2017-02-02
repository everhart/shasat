#ifndef BITOPS_H
#define BITOPS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

int bitVal32(uint32_t word, uint32_t pos);
int bitVal64(uint64_t word, uint32_t pos);
int bitPosRro(size_t wsize, uint32_t pos, uint32_t rro);
int bitPosLro(size_t wsize, uint32_t pos, uint32_t lro);

bool * nextCombination(bool *arr, size_t leng);

#endif
