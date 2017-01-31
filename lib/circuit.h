#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "./routines.h"

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

#endif