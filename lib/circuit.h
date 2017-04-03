#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <stdarg.h>
#include <stdlib.h>
#include "./types.h"
#include "./bitops.h"

int fwrite_atom(
	FILE *		stream,
	atom_t		atom
);

int fwrite_word32_atoms(
	FILE *		stream,
	index_t		lhs,
	uint32_t 	rhs
);

int fwrite_word64_atoms(
	FILE *		stream,
	index_t		lhs,
	uint64_t 	rhs
);

int fwrite_clauses(
	FILE *		    stream,
	const atom_t *	ante, 
	size_t 		    aleng, 
	const atom_t *  cons, 
	size_t 		    cleng
);

int fwrite_iff_clauses(
	FILE *		stream,
	size_t		wsize,
	index_t		lhs,
	index_t		rhs	
);

int fwrite_rsh_clauses(
	FILE *		stream,
	size_t		wsize,
	index_t		lhs,
	index_t		rhs,
	uint32_t	shift
);

int fwrite_lsh_clauses(
	FILE *		stream,
	size_t		wsize,
	index_t		lhs,
	index_t		rhs,
	uint32_t	shift
);

int fwrite_rro_clauses(
	FILE *		stream,
	size_t 		wsize,
	index_t		lhs,
	index_t		rhs,
	uint32_t	rot
);

int fwrite_lro_clauses(
	FILE *		stream,
	size_t 		wsize,
	index_t		lhs,
	index_t		rhs,
	uint32_t	rot
);

int fwrite_and_clauses(
    FILE *      stream,
    size_t      wsize,
    index_t     lhs,
    size_t      count,
    ...
);

int fwrite_sum_clauses(
	FILE *		stream,
	size_t		wsize,
	uint32_t	sum,
	atom_t		gen,
	uint32_t	count,
	...
);

#endif
