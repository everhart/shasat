#ifndef BITOPS_H
#define BITOPS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

int word32_bit(uint32_t word, size_t n);
int word64_bit(uint64_t word, size_t n);
int word32_assign_bit(uint32_t * word, size_t n, bool val);
int word64_assign_bit(uint32_t * word, size_t n, bool val);
ssize_t bit_position_rro(size_t wsize, size_t n, size_t rot);
ssize_t bit_position_lro(size_t wsize, size_t n, size_t rot);

bool * next_combination(bool *comb, size_t size);

#endif
