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
	uint32_t 	chunk, 
	uint32_t 	word,
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

int preprocessSHA1(
	FILE *		stream,
	size_t		msize
);

int sha1sat(
	FILE *		stream, 
	size_t 		msize, 
	const char * 	digest
);

#endif
