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

#endif
