#ifndef SHA1SAT_H
#define SHA1SAT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../shasat.h"

int sha1sat(
	FILE *		stream, 
	size_t 		msize, 
	const char * 	digest
);

#endif
