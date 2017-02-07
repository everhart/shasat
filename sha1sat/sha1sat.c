#include "./sha1sat.h"

static const uint32_t INDICES_PER_CHUNK = 38733;

typedef struct SHA1SAT {
	FILE *		stream;
	uint32_t	chunk;
	uint32_t	loop;
	index_t		generic;
	index_t		k[4];		//round constants
	index_t 	w[80];		//message schedule array
	index_t		cc[5];		//compressed chunk
	index_t		sig;
	index_t		ch;
	index_t		temp;	
	index_t		hh[5];		//hash
} SHA1SAT;

static index_t indexK(
	uint32_t ccount, uint32_t idx, uint32_t bit
);
static index_t indexMessage(
	uint32_t ccount, uint32_t idx, uint32_t bit
);
static index_t indexW(
	uint32_t chunk, uint32_t idx, uint32_t bit
);
static index_t indexCc( 
	uint32_t chunk, uint32_t kind, uint32_t idx, uint32_t bit
);
static index_t indexSig(
	uint32_t chunk, uint32_t idx, uint32_t bit
);
static index_t indexCh(
	uint32_t chunk, uint32_t idx, uint32_t bit
);
static index_t indexTemp(
	uint32_t chunk, uint32_t idx, uint32_t bit
);
static index_t indexHh(
	uint32_t chunk, uint32_t kind, uint32_t bit
);
static index_t indexGeneric(uint32_t chunk, uint32_t idx, uint32_t bit);

static int fwriteWClauses(SHA1SAT sha1sat);
static int fwriteSigClauses(SHA1SAT sha1sat);
static int fwriteChClauses(SHA1SAT sha1sat);
static int fwriteTempClauses(SHA1SAT * sha1sat);
static int fwriteCcClauses(SHA1SAT * sha1sat);
static int fwriteHhClauses(SHA1SAT * sha1sat);

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

static index_t indexCc(
	uint32_t chunk, uint32_t kind, uint32_t idx, uint32_t bit
) {
	return 0;
}

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

static index_t indexHh(uint32_t chunk, uint32_t kind, uint32_t bit) {
	return 0;
}

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

static int fwriteSigClauses(SHA1SAT shs) {
	int res = 0;
	bool comb[3] = { 0 },
	     eval = 0;

	atom_t ante[3] = { 0 },
	       cons = 0;

	for (int i = 0; i < (1 << 3); i++) {
		*comb = nextCombination(comb, 3);

		if (shs.loop > 0 && shs.loop < 20) {
			eval = (comb[0] & comb[1]) | 
			       (!comb[0] & comb[2]);
		}
		else if (shs.loop > 40 && shs.loop < 60) {
			eval = (comb[0] & comb[1]) | 
			       (comb[0] & comb[2]) | 
			       (comb[1] & comb[2]);
		}
		else {
			eval = (comb[0] ^ comb[1] ^ comb[2]);
		}

		for (int j = 0; j < 32; j++) {
			ante[0] = signAtom(shs.cc[1] + j, comb[0]);
			ante[1] = signAtom(shs.cc[2] + j, comb[1]);
			ante[2] = signAtom(shs.cc[3] + j, comb[2]);

			cons = signAtom(shs.sig + j, eval);

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

static int fwriteChClauses(SHA1SAT shs) {
	return fwriteLroClauses(
		shs.stream,
		32,
		shs.cc[0],
		shs.ch,
		5
	);
}

static int fwriteTempClauses(SHA1SAT * shs) {
	int res = fwriteSumClauses(
		shs->stream,
		32,
		shs->generic,
		5,
		shs->cc[4],
		shs->sig,
		shs->ch,
		shs->k[shs->loop / 20],
		shs->w[shs->loop]
	);
	if (res < 0) {
		return -1;
	}

	shs->generic = res;

	return 0;
}

static int fwriteCcClauses(SHA1SAT * shs) {
	int res = 0;

	for (int i = 4; i >= 0; i--) {
		shs->cc[4] = indexCc(shs->chunk, i, shs->loop + 1, 0);

		if (i == 2) {
			res = fwriteLroClauses(
				shs->stream,
				32,
				shs->cc[i - 1],
				shs->cc[i],
				30
			);
		}
		else {
			res = fwriteAssignClauses(
				shs->stream, 32,  shs->cc[i - 1], shs->cc[i]
			);
		}

		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

static int fwriteHhClauses(SHA1SAT * shs) {
	int res = 0;
	index_t hh[5] = { 0 };
	memcpy(hh, shs->hh, sizeof(hh));

	for (int i = 0; i < 5; i++) {
		shs->hh[i] = indexHh(shs->chunk, i, 0);

		res = fwriteSumClauses(
			shs->stream,
			32,
			shs->hh[i],
			shs->generic,
			2,
			hh[i],
			shs->cc[i]
		);
		if (res < 0) {
			return -1;
		}

		shs->generic = res;
	}

	return 0;
}

int sha1sat(FILE * stream, size_t msize, const char * digest) {
	return 0;
}

