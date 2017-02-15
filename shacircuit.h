#ifndef SHACIRCUIT_H
#define SHACIRCUIT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "./lib/atom.h"
#include "./lib/bitops.h"
#include "./lib/circuit.h"

int fwriteDigestAtomsSha(
	FILE * stream, const index_t * hh, size_t leng
);

size_t fwritePreprocClausesSha(
	FILE * stream, index_t message, size_t msize, size_t csize
);

int fwriteSigClausesSha(
	FILE * 		stream,
	size_t		wsize,
	index_t		sig,
	index_t		gen,
	index_t		x,
	uint32_t	r,
	uint32_t	s,
	uint32_t	t
);

int fwriteEpClausesSha(
	FILE *		stream,
	size_t		wsize,
	index_t		ep,
	index_t		x,
	uint32_t	r,
	uint32_t	s,
	uint32_t	t
);

int fwriteChClausesSha(
	FILE *		stream,
	size_t		wsize,
	index_t		ch,
	index_t		x,
	index_t		y,
	index_t		z
);

int fwriteParClausesSha(
	FILE *		stream,
	size_t		wsize,
	index_t		par,
	index_t		x,
	index_t		y,
	index_t		z
);

int fwriteMajClausesSha(
	FILE *		stream,
	size_t		wsize,
	index_t		maj,
	index_t		x,
	index_t		y,
	index_t		z
);

#endif
