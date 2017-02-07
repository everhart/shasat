#include "./sha1sat.h"

static const uint32_t INDICES_PER_CHUNK = 41416;
static const uint32_t CLAUSES_PER_CHUNK = 0;

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
	return INDICES_PER_CHUNK * chunk + 2561 + 
	       kind * 2592 + 
	       idx * 32 +
	       bit;
} //12960 cc indices

static index_t indexSig(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 15522 +
	       idx * 32 +
	       bit;
}	//2560 sig indices

static index_t indexCh(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 18083 +
	       idx * 32 +
	       bit;
}	//2560 ch indices

static index_t indexTemp(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 20644 +
	       idx * 32 +
	       bit;
}	//2560 temp indices

static index_t indexHh(uint32_t chunk, uint32_t kind, uint32_t bit) {
	return INDICES_PER_CHUNK + chunk + 23205 +
	       kind * 32 +
	       bit;
}	//160 hh indices

static index_t indexGeneric(uint32_t chunk, uint32_t idx, uint32_t bit) { 
	return INDICES_PER_CHUNK * chunk + 23336 + 
	       idx * 32 + 
	       bit;
}	//18080 generic indices

static int fwriteWClauses(SHA1SAT shs) {
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
				shs.w[shs.loop - 3] + j, 
				comb[0]
			);
			ante[1] = signAtom(
				shs.w[shs.loop - 8] + j, 
				comb[1]
			);
			ante[2] = signAtom(
				shs.w[shs.loop - 14] + j, 
				comb[2]
			);
			ante[3] = signAtom(
				shs.w[shs.loop - 16] + j, 
				comb[3]
			);

			cons = signAtom(
				shs.w[shs.loop] + bitPosLro(32, j, 1),
				eval
			);

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

static int fwriteKAtoms(SHA1SAT shs) {
	int res = 0;

	int k[4] = {
		0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6
	};

	for (int i = 0; i < 4; i++) {
		res = fwriteAtoms32(
			shs.stream, k[i], shs.k[i]
		);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

static int fwriteHhAtoms(SHA1SAT shs) {
	int res = 0;
	int hh[5] = {
		0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476,	0xC3D2E1F0
	};

	for (int i = 0; i < 4; i++) {
		res = fwriteAtoms32(
			shs.stream, hh[i], shs.hh[i]
		);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

int sha1sat(FILE * stream, size_t msize, const char * digest) {
	return 0;
}

