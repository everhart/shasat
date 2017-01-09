#ifndef SHA1SAT_H
#define SHA1SAT_H

#include <stdio.h>
#include <stdint.h>
#include "../lib/circuit.h"

#define SHA1_ATOMS_PER_CHUNK 35136

uint32_t indexHashBitSHA1(
	uint32_t 	chunk,
	uint32_t	word,
	uint32_t 	bit
);
uint32_t indexMessageScheduleBitSHA1(
	uint32_t	chunk,
	uint32_t	word,
	uint32_t	bit
);
uint32_t indexWorkingVarBitSHA1(
	uint32_t 	chunk,
	uint32_t 	word,
	uint32_t	bit
);
uint32_t indexBufferBitSHA1(
	uint32_t	chunk,
	uint32_t	word,
	uint32_t	bit
);
uint32_t indexRoundConstBitSHA1(
	size_t 		msize,
	uint32_t	word,
	uint32_t	bit
);
uint32_t indexDigestBitSHA1(
	size_t 		msize,
	uint32_t	bit
);
uint32_t indexMessageBitSHA1(
	size_t		msize,
	uint32_t	bit
);

#endif
