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
	index_t		SIG0;
	index_t		SIG1;
	index_t		cc[8];
	index_t		EP0;
	index_t		EP1;
	index_t		CH;
	index_t		MAJ;
	index_t		temp1;
	index_t		temp2;
	index_t		hh[8];	
} SHA256SAT;


