#ifndef SHACIRCUIT_H
#define SHACIRCUIT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "./lib/atom.h"
#include "./lib/bitops.h"
#include "./lib/circuit.h"

size_t fwritePreprocClausesSha(
	FILE * stream, index_t M, size_t msize, size_t csize
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

#endif
