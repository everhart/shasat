#ifndef SHA1SAT_H
#define SHA1SAT_H

#include <stdio.h>
#include <stdint.h>
#include "../lib/circuit.h"

uint32_t indexRndBitSHA1();
uint32_t indexHshBitSHA1();
uint32_t indexWvrBitSHA1();
uint32_t indexWrdBitSHA1();
uint32_t indexMsgBitSHA1();
uint32_t indexBufBitSHA1();

#endif
