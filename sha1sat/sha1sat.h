#ifndef SHA1SAT_H
#define SHA1SAT_H

#include <stdio.h>
#include <stdint.h>
#include "../lib/circuit.h"

#define SHA1_ATOMS_PER_CHUNK 35136

int sha1sat(
	FILE *		stream, 
	size_t 		msize, 
	const char * 	digest
);

#endif
