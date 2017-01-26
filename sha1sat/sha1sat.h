#ifndef SHA1SAT_H
#define SHA1SAT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../shasat.h"

#define SHA1_ATOMS_PER_CHUNK 35136

typedef struct HHSHA1 {
	uint32_t h0,
		 h1,
		 h2,
		 h3,
		 h4;
} HHSHA1;

typedef struct WVSHA1 {
	uint32_t a,
		 b,
		 c,
		 d,
		 e;
} WVSHA1;

int fwriteChunkClausesSHA1(
	FILE *		stream,
	uint32_t	inp,
	uint32_t	oup
);

int fwriteMessageScheduleClausesSHA1(
	FILE *		stream,
	uint32_t	inp[4],
	uint32_t	oup
);

int fwriteFClausesSHA1(
	FILE *		stream,
	uint32_t	inp[3],
	uint32_t	oup,
	uint32_t	idx
);

int fwriteTempClausesSHA1(
	FILE *		stream,
	uint32_t	inp[8],
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
