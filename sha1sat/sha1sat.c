#include "./sha1sat.h"

static const uint32_t INDICES_PER_CHUNK = 38733;

typedef struct SHA1SAT {
	FILE *	stream;
	index_t	chunk,
		loop,
		generic,
		k[4],
		w[80],
		a,
		b,
		c,
		d,
		e,
		sig,
		ch,
		temp,
		h0,
		h1,
		h2,
		h3,
		h4;
} SHA1SAT;

static index_t indexK(uint32_t ccount, uint32_t idx, uint32_t bit);
static index_t indexMessage(uint32_t ccount, uint32_t idx, uint32_t bit);
static index_t indexW(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexA(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexB(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexC(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexD(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexE(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexSig(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexCh(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexTemp(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexH0(uint32_t chunk, uint32_t bit);
static index_t indexH1(uint32_t chunk, uint32_t bit);
static index_t indexH2(uint32_t chunk, uint32_t bit);
static index_t indexH3(uint32_t chunk, uint32_t bit);
static index_t indexH4(uint32_t chunk, uint32_t bit);
static index_t indexGeneric(uint32_t chunk, uint32_t idx, uint32_t bit);

static int fwriteWClauses(SHA1SAT sha1sat);
static int fwriteFClauses(SHA1SAT sha1sat);
static int fwriteTempClauses(SHA1SAT sha1sat);
static int fwriteAClauses(SHA1SAT * sha1sat);
static int fwriteBClauses(SHA1SAT * sha1sat);
static int fwriteCClauses(SHA1SAT * sha1sat);
static int fwriteDClauses(SHA1SAT * sha1sat);
static int fwriteEClauses(SHA1SAT * sha1sat);
static int fwriteH0Clauses(SHA1SAT * sha1sat);
static int fwriteH1Clauses(SHA1SAT * sha1sat);
static int fwriteH2Clauses(SHA1SAT * sha1sat);
static int fwriteH3Clauses(SHA1SAT * sha1sat);
static int fwriteH4Clauses(SHA1SAT * sha1sat);

static index_t indexK(uint32_t ccount, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * ccount + 	//amount of previous indices
	       idx * 32 + 			//word index
	       bit;				//bit index
}	//128 k indices

static index_t indexMessage(uint32_t ccount, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * ccount + 128 +  
	       idx * 32 +
	       bit;	
}	//arbitrary amount of message indices

static index_t indexW(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 1 +
	       idx * 32 +
	       bit;
}	//2560 w indices

static index_t indexA(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 2561 +
	       idx * 32 +
	       bit;
}	//2560 a indices

static index_t indexB(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 5122 +
	       idx * 32 +
	       bit;
}	//2560 b indices

static index_t indexC(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 7683 +
	       idx * 32 +
	       bit;
}	//2560 c indices

static index_t indexD(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 10244 +
	       idx * 32 +
	       bit;
}	//2560 d indices

static index_t indexE(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 12805 +
	       idx * 32 +
	       bit;
}	//2560 e indices

static index_t indexSig(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 15366 +
	       idx * 32 +
	       bit;
}	//2560 SIG indices

static index_t indexTemp(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 17927 +
	       idx * 32 +
	       bit;
}	//2560 temp indices

static index_t indexH0(uint32_t chunk, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 20488 +
	       bit;
}	//32 h0 indices

static index_t indexH1(uint32_t chunk, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 20521 +
	       bit;
}	//32 h1 indices

static index_t indexH2(uint32_t chunk, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 20554 +
	       bit;	
}	//32 h2 indices

static index_t indexH3(uint32_t chunk, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 20587 +
	       bit;	
}	//32 h3 indices

static index_t indexH4(uint32_t chunk, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 20620 +
	       bit;	
}	//32 h4 indices

static index_t indexGeneric(uint32_t chunk, uint32_t idx, uint32_t bit) { 
	return INDICES_PER_CHUNK * chunk + 20653 + 
	       idx * 32 + 
	       bit;
}	//18080 generic indices

static int fwriteWClauses(SHA1SAT sha1sat) {
	int res = 0;
	bool comb[4] = { 0 },
	     eval = 0;

	atom_t ante[4] = { 0 },
	       cons = 0;

	for (int i = 0; i < (1 << 4); i++) {
		*comb = nextCombination(comb, 4);
		eval = comb[0] ^ comb[1] ^ comb[2] ^ comb[3];

		for (int j = 0; j < 32; j++) {
			ante[0] = signAtom(
				sha1sat.w[sha1sat.loop - 3] + j, 
				comb[0]
			);
			ante[1] = signAtom(
				sha1sat.w[sha1sat.loop - 8] + j, 
				comb[1]
			);
			ante[2] = signAtom(
				sha1sat.w[sha1sat.loop - 14] + j, 
				comb[2]
			);
			ante[3] = signAtom(
				sha1sat.w[sha1sat.loop - 16] + j, 
				comb[3]
			);

			cons = signAtom(
				sha1sat.w[sha1sat.loop] + bitPosLro(32, j, 1),
				eval
			);

			res = fwriteClauses(
				sha1sat.stream, ante, 3, &cons, 1
			);
			if (res < 0) {
				return -1;
			}

		}
	}

	return 0;
}

static int fwriteSigClauses(SHA1SAT sha1sat) {
	int res = 0;
	bool comb[3] = { 0 },
	     eval = 0;

	atom_t ante[3] = { 0 },
	       cons = 0;

	for (int i = 0; i < (1 << 3); i++) {
		*comb = nextCombination(comb, 3);

		if (sha1sat.loop > 0 && sha1sat.loop < 20) {
			eval = (comb[0] & comb[1]) | 
			       (!comb[0] & comb[2]);
		}
		else if (sha1sat.loop > 40 && sha1sat.loop < 60) {
			eval = (comb[0] & comb[1]) | 
			       (comb[0] & comb[2]) | 
			       (comb[1] & comb[2]);
		}
		else {
			eval = (comb[0] ^ comb[1] ^ comb[2]);
		}

		for (int j = 0; j < 32; j++) {
			ante[0] = signAtom(sha1sat.b + j, comb[0]);
			ante[1] = signAtom(sha1sat.c + j, comb[1]);
			ante[2] = signAtom(sha1sat.d + j, comb[2]);

			cons = signAtom(sha1sat.sig + j, eval);

			res = fwriteClauses(
				sha1sat.stream, ante, 3, &cons, 1
			);
			if (res < 0) {
				return -1;
			}

		}
	}

	return 0;
}

static int fwriteTempClauses(SHA1SAT sha1sat) {
	return fwriteSumClauses(
		sha1sat.stream,
		32,
		sha1sat.generic,
		5,
		sha1sat.e,
		sha1sat.sig,
		sha1sat.ch,
		sha1sat.k[sha1sat.loop / 20],
		sha1sat.w[sha1sat.loop]
	);
}

static int fwriteEClauses(SHA1SAT * sha1sat) { 
	int res = 0;

	sha1sat->e = indexE(sha1sat->chunk, sha1sat->loop, 0);
	
	res = fwriteAssignClauses(
		sha1sat->stream, 32, sha1sat->d, sha1sat->e
	);
	if (res < 0) {
		return -1;
	}

	return 0;
}

static int fwriteDClauses(SHA1SAT * sha1sat) {
	int res = 0;

	sha1sat->d = indexD(sha1sat->chunk, sha1sat->loop, 0);
	
	res = fwriteAssignClauses(
		sha1sat->stream, 32, sha1sat->c, sha1sat->d
	);
	if (res < 0) {
		return -1;
	}

	return 0;
}

static int fwriteCClauses(SHA1SAT * sha1sat) {
	int res = 0; 

	sha1sat->c = indexC(sha1sat->chunk, sha1sat->loop, 0);

	res = fwriteLroClauses(
		sha1sat->stream, 32, sha1sat->b, sha1sat->c, 30
	);
	if (res < 0) {
		return -1;
	}

	return 0;
}

static int fwriteBClauses(SHA1SAT * sha1sat) {
	int res = 0; 

	sha1sat->b = indexB(sha1sat->chunk, sha1sat->loop, 0);

	res = fwriteAssignClauses(
		sha1sat->stream, 32, sha1sat->a, sha1sat->b
	);
	if (res < 0) {
		return -1;
	}

	return 0;
}

static int fwriteAClauses(SHA1SAT * sha1sat) {
	int res = 0;

	sha1sat->a = indexA(sha1sat->chunk, sha1sat->loop, 0);

	res = fwriteAssignClauses(
		sha1sat->stream, 32, sha1sat->temp, sha1sat->a
	);
	if (res < 0) {
		return -1;
	}

	return 0;
}

int sha1sat(FILE * stream, size_t msize, const char * digest) {
	return 0;
}

