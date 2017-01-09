#ifndef SHA1SAT_H
#define SHA1SAT_H

#include <stdio.h>
#include <stdint.h>
#include "../lib/circuit.h"

uint32_t indexMessageBitSHA1();
uint32_t indexHashBitSHA1();
uint32_t indexRoundConstBitSHA1();
uint32_t indexMessageScheduleBitSHA1();
uint32_t indexWorkingVarBitSHA1();
uint32_t indexBufferBitSHA1();

#endif
