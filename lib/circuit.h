#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
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

int fwrite_xor_clauses(
    FILE *      stream,
    size_t      wsize,
    index_t     lhs,
    size_t      count,
    ...
);

int fwrite_or_clauses(
    FILE *      stream,
    size_t      wsize,
    index_t     lhs,
    size_t      count,
    ...
);

atom_t fwrite_sum_clauses(
	FILE *		stream,
	size_t		wsize,
	index_t	    lhs,
	index_t		gen,
	size_t	    count,
	...
);

int fwrite_sha_digest_word32_atoms(
    FILE * stream, const index_t * hh, const char * digest, size_t size
);

int fwrite_sha_digest_word64_atoms(
    FILE * stream, const index_t * hh, const char * digest, size_t size
);

int fwrite_sha_preprocessing_atoms(
    FILE * stream, index_t msg, size_t msize, size_t csize
);

int fwrite_sig_clauses(
    FILE *      stream,
    size_t      wsize,
    index_t     lhs,
    index_t     gen,
    index_t     rhs,
    size_t      rot1,
    size_t      rot2,
    size_t      shift
);

int fwrite_ep_clauses(
    FILE *      stream,
    size_t      wsize,
    index_t     lhs,
    index_t     rhs,    
    size_t      rot1,
    size_t      rot2,
    size_t      rot3
);

int fwrite_ch_clauses(
    FILE *      stream,
    size_t      wsize,
    index_t     lhs,
    index_t     rhs1,
    index_t     rhs2,
    index_t     rhs3
);

#endif
