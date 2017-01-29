#ifndef BITOPS_H
#define BITOPS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

int bitValue32(uint32_t word, uint32_t pos);
int bitValue64(uint64_t word, uint32_t pos);

uint32_t bitPositionRsh(uint32_t wsize, uint32_t pos, uint32_t rsh);
uint32_t bitPositionRro(uint32_t wsize, uint32_t pos, uint32_t rro);
uint32_t bitPositionLsh(uint32_t wsize, uint32_t pos, uint32_t lsh);
uint32_t bitPositionLro(uint32_t wsize, uint32_t pos, uint32_t Lro);

#endif
