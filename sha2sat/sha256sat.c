#include "sha256sat.h"

typedef struct SHA256SAT {
	FILE *		stream;
	char *		digest;	
	uint32_t	chunk;
	uint32_t	loop;
	index_t		generic;
	index_t		message;
	index_t		k[64];
	index_t		w[64];
	index_t		sig0;
	index_t		sig1;
	index_t		cc[8];
	index_t		ep0;
	index_t		ep1;
	index_t		ch;
	index_t		maj;
	index_t		temp1;
	index_t		temp2;
	index_t		hh[8];	
} SHA256SAT;
