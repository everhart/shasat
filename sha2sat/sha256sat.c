#include "sha256sat.h"

static const uint32_t INDICES_PER_CHUNK = 0;
static const uint32_t CLAUSES_PER_CHUNK = 0;

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

static index_t indexK(uint32_t ccount, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * ccount +
	       idx * 32 +
	       bit;
}

static index_t indexMessage(
	uint32_t ccount, uint32_t chunk, uint32_t idx, uint32_t bit
) {
	return INDICES_PER_CHUNK * ccount + 2049 +
	       chunk * 512 +
	       idx * 32 +
	       bit;
}

static index_t indexW(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 1 +
	       idx * 32 + 
	       bit;
}	//2048 w indices

static index_t indexSIG0(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 1 + 
	       idx * 32 + 
	       bit;
}	//2048 SIG0 indices

static index_t indexSIG1(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 2049 +
	       idx * 32 +
	       bit;
}	//2048 SIG1 indices

static index_t indexCc(
	uint32_t chunk, uint32_t kind, uint32_t idx, uint32_t bit
) {
	return INDICES_PER_CHUNK * chunk + 4098 +
	       idx * 32 + 
	       bit;
}	//16384 cc indices

static index_t indexEP0(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 20483 +
	       idx * 32 +
	       bit;
}	//2048 EP0 indices

static index_t indexEP1(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 22532 +
	       idx * 32 + 
	       bit;
}	//2048 EP1 indices

static index_t indexCH(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 24581 +
	       idx * 32 +
	       bit;
}	//2048 CH indices


