#include "sha256sat.h"

static const uint32_t INDICES_PER_CHUNK = 59499;
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
	return INDICES_PER_CHUNK * chunk + 2049 + 
	       idx * 32 + 
	       bit;
}	//1536 SIG0 indices

static index_t indexSIG1(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 3586 +
	       idx * 32 +
	       bit;
}	//1536 SIG1 indices

static index_t indexCc(
	uint32_t chunk, uint32_t kind, uint32_t idx, uint32_t bit
) {
	return INDICES_PER_CHUNK * chunk + 5123 +
	       idx * 32 + 
	       bit;
}	//16384 cc indices

static index_t indexEP0(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 21508 +
	       idx * 32 +
	       bit;
}	//2048 EP0 indices

static index_t indexEP1(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 23557 +
	       idx * 32 + 
	       bit;
}	//2048 EP1 indices

static index_t indexCH(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 25606 +
	       idx * 32 +
	       bit;
}	//2048 CH indices

static index_t indexMAJ(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 27655 +
	       idx * 32 + 
	       bit;
}	//2048 MAJ indices

static index_t indexTemp1(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 29704 +
	       idx * 32 +
	       bit;
}	//2048 temp1 indices

static index_t indexTemp2(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 31753 +
	       idx * 32 +
	       bit;
}	//2048 temp2 indices

static index_t indexHh(uint32_t chunk, uint32_t type, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 33802 +
	       type * 32 +
       	       bit;	       
}	//256 hh indices

static index_t indexGeneric(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 34059 +
	       idx * 32 + 
	       bit;
}	//25440 generic indices

static int fwriteSIG0Clauses(SHA256SAT * shs) {
	int res = 0;
	bool comb[3] = { 0 },
	     eval = 0;

	atom_t ante[3] = { 0 },
	       cons = 0;

	res = fwriteRshClauses(
		shs->stream, 
		32,
		shs->w[shs->loop - 15],
		shs->generic,
		3
	);
	if (res < 0) {
		return -1;
	}

	for (int i = 0; i < (1 << 3); i++) {
		*comb = nextCombination(comb, 3);
		eval = comb[0] ^ comb[1] ^ comb[2];

		for (int j = 0; j < 32; j++) {
			ante[0] = signAtom(
				shs->w[shs->loop - 15] + bitPosRro(32, j, 7),
				comb[0]
			);
			ante[1] = signAtom(
				shs->w[shs->loop - 15] + bitPosRro(32, j, 18),
				comb[1]
			);

			ante[2] = signAtom(shs->generic + j, comb[2]);
			cons = signAtom(shs->SIG0 + j, eval);

			res = fwriteClauses(
				shs->stream, ante, 3, &cons, 1
			);
			if (res < 0) {
				return -1;
			}
		}
	}

	return 0;
}
