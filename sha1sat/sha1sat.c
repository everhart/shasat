#include "./sha1sat.h"

static const uint32_t INDICES_PER_CHUNK = 41286;
static const uint32_t CLAUSES_PER_CHUNK = 0;

typedef struct Sha1Sat {
	FILE *		stream;
	uint32_t	chunk;
	uint32_t	loop;
	index_t		generic;
	index_t		message;
	index_t		k[4];		//round constants
	index_t 	w[80];		//message schedule array
	index_t		cc[5];		//compressed chunk
	index_t		f;
	index_t		g;
	index_t		temp;	
	index_t		hh[5];		//hash
} Sha1Sat;

static index_t indexKSha1(uint32_t ccount, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * ccount + 	//amount of previous indices
	       idx * 32 + 			//word index
	       bit;				//bit index
}	//128 k indices

static index_t indexMessageSha1(
	uint32_t ccount, uint32_t chunk, uint32_t idx, uint32_t bit
) {
	return INDICES_PER_CHUNK * ccount + 129 + 
	       chunk * 512 +
	       idx * 32 +
	       bit;	
}	//arbitrary amount of message indices

static index_t indexWSha1(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 1 +
	       idx * 32 +
	       bit;
}	//2560 w indices

static index_t indexCcSha1(
	uint32_t chunk, uint32_t kind, uint32_t idx, uint32_t bit
) {
	return INDICES_PER_CHUNK * chunk + 2561 + 
	       kind * 2592 + 
	       idx * 32 +
	       bit;
} //12800 cc indices

static index_t indexFSha1(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 15362 +
	       idx * 32 +
	       bit;
}	//2560 sig indices

static index_t indexGSha1(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 17923 +
	       idx * 32 +
	       bit;
}	//2560 ch indices

static index_t indexTempSha1(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 20484 +
	       idx * 32 +
	       bit;
}	//2560 temp indices

static index_t indexHhSha1(uint32_t chunk, uint32_t kind, uint32_t bit) {
	return INDICES_PER_CHUNK + chunk + 23045 +
	       kind * 32 +
	       bit;
}	//160 hh indices

static index_t indexGenericSha1(uint32_t chunk, uint32_t idx, uint32_t bit) { 
	return INDICES_PER_CHUNK * chunk + 23206 + 
	       idx * 32 + 
	       bit;
}	//18080 generic indices

static int fwriteMessageClausesSha1(Sha1Sat shs) {
	return fwriteAssignClauses(
		shs.stream,
		32,
		shs.w[shs.loop],
		shs.message
	);
}

static int fwriteWClausesSha1(Sha1Sat shs) {
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

static int fwriteFClausesSha1(Sha1Sat shs) {
	int res = 0;
	
	if (shs.loop > 0 && shs.loop < 20) {
		res = fwriteChClausesSha(
			shs.stream, 
			32, 
			shs.f,
			shs.cc[1], 
			shs.cc[2], 
			shs.cc[3]
		);
	}
	else if (shs.loop > 40 && shs.loop < 60) {
		res = fwriteMajClausesSha(
			shs.stream, 
			32, 
			shs.f,
			shs.cc[1], 
			shs.cc[2], 
			shs.cc[3]
		);
	}
	else {
		res = fwriteParClausesSha(
			shs.stream, 
			32,
			shs.f,
			shs.cc[1], 
			shs.cc[2], 
			shs.cc[3]
		);
	}

	return 0;
}

static int fwriteGClausesSha1(Sha1Sat shs) {
	return fwriteLroClauses(
		shs.stream,
		32,
		shs.cc[0],
		shs.g,
		5
	);
}

static int fwriteTempClausesSha1(Sha1Sat * shs) {
	int res = fwriteSumClauses(
		shs->stream,
		32,
		shs->temp,
		shs->generic,
		5,
		shs->cc[4],
		shs->f,
		shs->g,
		shs->k[shs->loop / 20],
		shs->w[shs->loop]
	);
	if (res < 0) {
		return -1;
	}

	shs->generic = res;

	return 0;
}

static int fwriteCcClausesSha1(Sha1Sat * shs) {
	int res = 0;

	for (int i = 4; i >= 0; i--) {
		shs->cc[i] = indexCc(shs->chunk, i, shs->loop + 1, 0);

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

static int fwriteHhClausesSha1(Sha1Sat * shs) {
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

static int fwriteKAtomsSha1(Sha1Sat shs) {
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

static int fwriteHhAtomsSha1(Sha1Sat shs) {
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
	//preprocess SHA
	msize = preprocessSHA(stream, msize, 512);
	if (msize == 0) {
		return -1;
	}

	int res = 0;
	const uint32_t ccount = msize / 512;
	Sha1Sat shs = {
		stream,	
		0,
		0,
		0,
		msize,
		{ 0 },		
		{ 0 },
		{ 0 },
		0,
		0,
		0,
		{ 0 }
	};
	
	//initialize k indices
	for (int i = 0; i < 4; i++) {
		shs.k[i] = indexK(ccount, i, 0);
	}

	//initialize hh indices
	for (int i = 0; i < 4; i++) {
		shs.hh[i] = indexHh(shs.chunk, i, 0);
	}

	//write the dimacs file header
	res = fprintf(
		stream, "p cnf %lu %lu \n",
		INDICES_PER_CHUNK * ccount + 128 + msize,
		CLAUSES_PER_CHUNK * ccount + 128 + msize
	);
	if (res < 0) {
		return -1;
	}

	//write atoms representing round constant values
	res = fwriteKAtoms(shs);
	if (res < 0) {
		return -1;
	}

	//write atoms representing initial hash values
	res = fwriteHhAtoms(shs);
	if (res < 0) {
		return -1;
	}

	//write clauses representing each and every round of SHA-1
	for (shs.chunk = 0; shs.chunk < ccount; shs.chunk++) {

		//a = h0 ... e = h4
		for (int i = 0; i < 5; i++) {
			shs.cc[i] = shs.hh[i];
		}

		for (shs.loop = 0; shs.loop < 80; shs.loop++) {
			//determine all loop based indices
			shs.w[shs.loop] = indexW(shs.chunk, shs.loop, 0);
			shs.f = indexF(shs.chunk, shs.loop, 0);
			shs.g = indexG(shs.chunk, shs.loop, 0);
			shs.temp = indexTemp(shs.chunk, shs.loop, 0);

			//break chunk into sixteen 32-bit words
			if (shs.loop < 16) {
				shs.message = indexMessage(
					ccount, shs.chunk, shs.loop, 0
				);

				if (fwriteMessageClauses(shs) < 0) {
					return -1;
				}	
			}
			//key extension	
			else {
				if (fwriteWClauses(shs) < 0) {
					return -1;
				}
			}

			//compression function
			if (
				(fwriteSigClauses(shs) < 0) ||
				(fwriteChClauses(shs) < 0) ||
				(fwriteTempClauses(&shs) < 0) ||
				(fwriteCcClauses(&shs) < 0)
			) {
				return -1;
			}
		}
	
		//update hash values
		if (fwriteHhClauses(&shs) < 0) {
			return -1;
		}
	}

	return 0;
}
