#ifndef SHA1SAT_H
#define SHA1SAT_H

#include <stdio.h>
#include <stdint.h>
#include "../lib/circuit.h"

uint32_t indexAndBitSHA1();
uint32_t indexNotAndBitSHA1();
uint32_t indexOrBitSHA1();
uint32_t indexXorBitSHA1();
uint32_t indexLeftRotateBitSHA1();
uint32_t indexSumBitSHA1();

#endif
