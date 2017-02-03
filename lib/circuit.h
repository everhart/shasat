#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "./atom.h"
#include "./bitops.h"

int fwriteAtom(
	FILE *		stream,
	atom_t		atom
);

int fwriteAtoms32(
	FILE *		stream,
	const uint32_t	rhs[],
	const index_t 	lhs[],
	size_t		leng
);

int fwriteAtoms64(
	FILE *		stream,
	const uint64_t	rhs[],
	const index_t 	lhs[],
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
	index_t		rhs,
	index_t		lhs,
	uint32_t	shift
);

int fwriteLroClauses(
	FILE *		stream,
	size_t 		wsize,
	index_t		rhs,
	index_t		lhs,
	uint32_t	rot
);

int fwriteSumClauses(
	FILE *		stream,
	size_t		wsize,
	index_t		a,
	index_t		b,
	index_t		sum,
	index_t		crr
);

#endif
