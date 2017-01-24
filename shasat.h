#ifndef SHASAT_H
#define SHASAT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int fwriteAtoms32(
	FILE *		stream,
	const uint32_t	inp[],
	const uint32_t 	oup[],
	uint64_t	leng
);

int fwriteAtoms64(
	FILE *		stream,
	const uint32_t	inp[],
	const uint64_t 	oup[],
	uint64_t	leng
);

int preprocessSHA(
	FILE *		stream,
	size_t		msize,
	size_t		csize
);

#endif
