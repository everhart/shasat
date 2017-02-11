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
	uint8_t		rhs,
	index_t 	lhs
);

int fwriteAtoms32(
	FILE *		stream,
	uint32_t	rhs,
	index_t 	lhs
);

int fwriteAtoms64(
	FILE *		stream,
	uint64_t	rhs,
	index_t 	lhs
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
	uint32_t	sum,
	atom_t		gen,
	uint32_t	count,
	...
);

#endif
