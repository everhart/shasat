#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <stdarg.h>
#include "./atom.h"
#include "./bitops.h"

int fwriteAtom(
	FILE *		stream,
	atom_t		atom
);

int fwriteAtoms8(
	FILE *		stream,
	index_t		lhs,
	uint8_t 	rhs
);

int fwriteAtoms32(
	FILE *		stream,
	index_t		lhs,
	uint32_t 	rhs
);

int fwriteAtoms64(
	FILE *		stream,
	index_t		lhs,
	uint64_t 	rhs
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
	index_t		lhs,
	index_t		rhs	
);

int fwriteRshClauses(
	FILE *		stream,
	size_t		wsize,
	index_t		lhs,
	index_t		rhs,
	uint32_t	shift
);

int fwriteLroClauses(
	FILE *		stream,
	size_t 		wsize,
	index_t		lhs,
	index_t		rhs,
	uint32_t	rot
);

int fwriteSumClauses(
	FILE *		stream,
	size_t		wsize,
	uint32_t	sum,
	atom_t		gen,
	uint32_t	count,
	...
);

#endif
