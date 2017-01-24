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
	uint32_t	word,
	uint32_t	w[80],
	uint32_t 	idx
);

int fwriteMessageScheduleClausesSHA1(
	FILE *		stream,
	uint32_t	w[80],
	uint32_t 	idx
);

int fwriteFClausesSHA1(
	FILE *		stream,
	WVSHA1		wv,
	uint32_t	f,
	uint32_t	idx
);

int fwriteTempClausesSHA1(
	FILE *		stream,
	WVSHA1		wv,
	uint32_t	f,
	uint32_t	k,
	uint32_t	w[80],
	uint32_t	temp
);

int sha1sat(
	FILE *		stream, 
	size_t 		msize, 
	const char * 	digest
);

#endif
