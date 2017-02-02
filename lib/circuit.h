#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "./atom.h"
#include "./bitops.h"

int fwriteAtoms32(
	FILE *		stream,
	const uint32_t	inp[],
	const index_t 	oup[],
	size_t		leng
);

int fwriteAtoms64(
	FILE *		stream,
	const uint64_t	inp[],
	const index_t 	oup[],
	size_t		leng
);

int fwriteClauses(
	FILE *		stream,
	const atom_t 	ante[], 
	size_t 		aleng, 
	const atom_t  	cons[], 
	size_t 		cleng
);

int fwriteAssignClauses(
	FILE *		stream,
	size_t		wsize,
	index_t		inp,
	index_t		oup	
);

int fwriteRshClauses(
	FILE *		stream,
	size_t		wsize,
	index_t		inp,
	index_t		oup,
	uint32_t	rsh
);

int fwriteLroClauses(
	FILE *		stream,
	size_t 		wsize,
	index_t		inp,
	index_t		oup,
	uint32_t	lro
);

int fwriteSumClauses(
	FILE *		stream,
	size_t		wsize,
	index_t		inp1,
	index_t		inp2,
	index_t		sum,
	index_t		crr
);

int fwriteLroSumClauses(
	FILE *		stream,
	size_t		wsize,
	index_t		inp1,
	index_t		inp2,
	index_t		sum,
	index_t		crr,
	uint32_t	lro
);

#endif
