#ifndef SHASAT_H
#define SHASAT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int fwriteAtoms(
	FILE *		stream,
	const uint32_t	inp[],
	const uint32_t 	oup[],
	size_t		size
);

#endif
