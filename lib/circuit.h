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

int fwriteClauses(
	FILE *		stream,
	int * 		ante, 
	size_t 		aleng, 
	int * 		cons, 
	size_t 		cleng
);

int fwriteAssignClauses(
	FILE *		stream,
	size_t		wsize,
	uint32_t	inp,
	uint32_t	oup	
);

int fwriteRshClauses(
	FILE *		stream,
	size_t		wsize,
	uint32_t	inp,
	uint32_t	oup,
	uint32_t	rsh
);

int fwriteSumClauses(
	FILE *		stream,
	size_t		wsize,
	uint32_t	inp1,
	uint32_t	inp2,
	uint32_t	sum,
	uint32_t	crr
);

int fwriteLroSumClauses(
	FILE *		stream,
	size_t		wsize,
	uint32_t	inp1,
	uint32_t	inp2,
	uint32_t	sum,
	uint32_t	crr,
	uint32_t	lro
);

#endif
