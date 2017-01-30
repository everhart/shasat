#ifndef ROUTINES_H
#define ROUTINES_H

#include <stdio.h>
#include <stdint.h>

int bitValue32(uint32_t word, uint32_t pos);
int bitValue64(uint64_t word, uint32_t pos);
int bitPositionRro(size_t wsize, uint32_t pos, uint32_t rro);
int bitPositionLro(size_t wsize, uint32_t pos, uint32_t lro);
inline int signAtom(int atom, int state);
inline int * nextPermutation(int *arr, size_t leng);

#endif