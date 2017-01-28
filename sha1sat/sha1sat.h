#ifndef SHA1SAT_H
#define SHA1SAT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../shasat.h"

#define SHA1_ATOMS_PER_CHUNK 35136

typedef struct WvSha1 {
	uint32_t a,
		 b,
		 c, 
		 d,
		 e;
} WvSha1;

typedef struct HhSha1 {
	uint32_t h0,
		 h1,
		 h2, 
		 h3,
		 h4;
} HhSha1;

uint32_t indexKBitSha1(
	uint32_t word, uint32_t bit
);
uint32_t indexWBitSha1(
	uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexWvBitSha1(
	uint32_t type, uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexHhBitSha1(
	uint32_t type, uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexFBitSha1(
	uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexTempBitSha1(
	uint32_t chunk, uint32_t word, uint32_t bit
);

int fwriteMessageScheduleClausesSha1(
	FILE *		stream,
	uint32_t	ms[80],
	uint32_t	idx
);

int fwriteFClausesSha1(
	FILE *		stream,
	WvSha1		wv,
	uint32_t	idx
);

int fwriteTempClausesSha1(
	FILE *		stream,
	WvSha1		wv,
	uint32_t 	f,
	uint32_t 	k,
	uint32_t	ms,
	uint32_t 	temp,
	uint32_t	inc
);

int fwriteWorkingVariableClausesSHA1(
	FILE *		stream,
	WvSha1 *	wv
);

int fwriteHashClausesSha1(
	FILE *		stream,
	HhSha1 *	hh,
	WvSha1		wv,
	uint32_t 	inc
);

int sha1sat(
	FILE *		stream, 
	size_t 		msize, 
	const char * 	digest
);

#endif
