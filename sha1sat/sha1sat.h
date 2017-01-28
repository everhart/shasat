#ifndef SHA1SAT_H
#define SHA1SAT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../shasat.h"

#define SHA1_ATOMS_PER_CHUNK 35136

uint32_t indexKBitSHA1(
	uint32_t word, uint32_t bit
);
uint32_t indexWBitSHA1(
	uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexWvBitSHA1(
	uint32_t type, uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexHhBitSHA1(
	uint32_t type, uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexFBitSHA1(
	uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexTempBitSHA1(
	uint32_t chunk, uint32_t word, uint32_t bit
);

int fwriteMessageScheduleClausesSHA1(
	FILE *		stream,
	uint32_t	w[80],
	uint32_t	idx
);

int fwriteFClausesSHA1(
	FILE *		stream,
	uint32_t	inp[3],
	uint32_t	oup,
	uint32_t	idx
);

int fwriteTempClausesSHA1(
	FILE *		stream,
	uint32_t	inp[5],
	uint32_t 	oup[8]
);

int fwriteWorkingVariableClausesSHA1(
	FILE *		stream,
	uint32_t	inp[5],
	uint32_t	oup[5]
);

int fwriteHashClausesSHA1(
	FILE *		stream,
	uint32_t	inp[10],
	uint32_t	oup[10]
);

int sha1sat(
	FILE *		stream, 
	size_t 		msize, 
	const char * 	digest
);

#endif
