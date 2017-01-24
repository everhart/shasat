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

int sha1sat(
	FILE *		stream, 
	size_t 		msize, 
	const char * 	digest
);

#endif
