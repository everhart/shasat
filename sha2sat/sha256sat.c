#include "sha256sat.h"

static const uint32_t INDICES_PER_CHUNK = 59499;
static const uint32_t CLAUSES_PER_CHUNK = 0;

typedef struct Sha256Sat {
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
} Sha256Sat;

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

static index_t indexSig0(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 2049 + 
	       idx * 32 + 
	       bit;
}	//1536 sig0 indices

static index_t indexSig1(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 3586 +
	       idx * 32 +
	       bit;
}	//1536 sig1 indices

static index_t indexCc(
	uint32_t chunk, uint32_t kind, uint32_t idx, uint32_t bit
) {
	return INDICES_PER_CHUNK * chunk + 5123 +
	       idx * 32 + 
	       bit;
}	//16384 cc indices

static index_t indexEp0(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 21508 +
	       idx * 32 +
	       bit;
}	//2048 ep0 indices

static index_t indexEp1(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 23557 +
	       idx * 32 + 
	       bit;
}	//2048 ep1 indices

static index_t indexCh(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 25606 +
	       idx * 32 +
	       bit;
}	//2048 ch indices

static index_t indexMaj(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 27655 +
	       idx * 32 + 
	       bit;
}	//2048 maj indices

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

static int fwriteSig0Clauses(Sha256Sat * shs) {
	int res = fwriteSigClausesSha(
		shs->stream,
		32,
		shs->sig0,
		shs->generic,
		shs->w[shs->loop - 15],
		7, 18, 3
	);
	if (res < 0) {
		return -1;
	}

	shs->generic = res;

	return 0;
}

static int fwriteSIG1Clauses(SHA256SAT * shs) {
	int res = 0;
	bool comb[3] = { 0 },
	     eval = 0;

	atom_t ante[3] = { 0 },
	       cons = 0;

	const index_t w = shs->w[shs->loop - 2];

	res = fwriteRshClauses(
		shs->stream, 
		32,
		w,
		shs->generic,
		10
	);
	if (res < 0) {
		return -1;
	}

	for (int i = 0; i < (1 << 3); i++) {
		*comb = nextCombination(comb, 3);
		eval = comb[0] ^ comb[1] ^ comb[2];

		for (int j = 0; j < 32; j++) {
			ante[0] = signAtom(
				w + bitPosRro(32, j, 17), comb[0]
			);
			ante[1] = signAtom(
				w + bitPosRro(32, j, 19), comb[1]
			);
			ante[2] = signAtom(shs->generic + j, comb[2]);
			
			cons = signAtom(shs->SIG1 + j, eval);

			res = fwriteClauses(
				shs->stream, ante, 3, &cons, 1
			);
			if (res < 0) {
				return -1;
			}
		}
	}

	shs->generic += 32;

	return 0;
}
static int fwriteWClauses(SHA256SAT * shs) {
	int res = fwriteSumClauses(
		shs->stream,
		32,
		shs->w[shs->loop],
		shs->generic,
		4,
		shs->w[shs->loop - 16],
		shs->SIG0,
		shs->w[shs->loop - 7],
		shs->SIG1
	);
	if (res < 0) {
		return -1;
	}

	shs->generic = res;

	return 0;
}

static int fwriteEP0Clauses(SHA256SAT shs) {
	int res = 0;
	bool comb[3] = { 0 },
	     eval = 0;

	atom_t ante[3] = { 0 },
	       cons = 0;

	for (int i = 0; i < (1 << 3); i++) {
		*comb = nextCombination(comb, 3);
		eval = (comb[0] ^ comb[1] ^ comb[2]);

		for (int j = 0; j < 32; j++) {
			ante[0] = signAtom(
				shs.cc[4] + bitPosRro(32, j, 6), 
				comb[0]
			);
			ante[1] = signAtom(
				shs.cc[4] + bitPosRro(32, j, 11), 
				comb[1]
			);
			ante[2] = signAtom(
				shs.cc[4] + bitPosRro(32, j, 25), 
				comb[1]
			);

			cons = signAtom(shs.EP0 + j, eval);

			res = fwriteClauses(
				shs.stream, ante, 3, &cons, 1
			);
			if (res < 0) {
				return -1;
			}
		}	
	}

	return 0;
}

static int fwriteCHClauses(SHA256SAT shs) {
	int res = 0;
	bool comb[3] = { 0 },
	     eval = 0;

	atom_t ante[3] = { 0 },
	       cons = 0;

	for (int i = 0; i < (1 << 3); i++) {
		*comb = nextCombination(comb, 3);
		eval = (comb[0] & comb[1]) ^
		       (!comb[0] & comb[2]);

		for (int j = 0; j < 32; j++) {
			ante[0] = signAtom(shs.cc[4] + j, comb[0]);
			ante[1] = signAtom(shs.cc[5] + j, comb[1]);
			ante[2] = signAtom(shs.cc[6] + j, comb[2]);

			cons = signAtom(shs.CH + j, eval);

			res = fwriteClauses(
				shs.stream, ante, 3, &cons, 1
			);
			if (res < 0) {
				return -1;
			}
		}	
	}

	return 0;
}

static int fwriteMAJClauses(SHA256SAT shs) {
	int res = 0;
	bool comb[3] = { 0 },
	     eval = 0;

	atom_t ante[3] = { 0 },
	       cons = 0;

	for (int i = 0; i < (1 << 3); i++) {
		*comb = nextCombination(comb, 3);
		eval = (comb[0] & comb[1]) ^
		       (comb[0] & comb[2]) ^
		       (comb[1] & comb[2]);

		for (int j = 0; j < 32; j++) {
			ante[0] = signAtom(shs.cc[0] + j, comb[0]);
			ante[1] = signAtom(shs.cc[1] + j, comb[1]);
			ante[2] = signAtom(shs.cc[2] + j, comb[2]);

			cons = signAtom(shs.MAJ + j, eval);

			res = fwriteClauses(
				shs.stream, ante, 3, &cons, 1
			);
			if (res < 0) {
				return -1;
			}
		}	
	}

	return 0;
}
