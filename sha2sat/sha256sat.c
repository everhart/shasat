#include "sha256sat.h"

static const uint32_t INDICES_PER_CHUNK = 59499;
static const uint32_t CLAUSES_PER_CHUNK = 0;

typedef struct Sha256Sat {
	FILE *		stream;
	const char *	digest;
	size_t		dsize;
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

static index_t indexKSha256(uint32_t ccount, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * ccount +
	       idx * 32 +
	       bit;
}

static index_t indexMessageSha256(
	uint32_t ccount, uint32_t chunk, uint32_t idx, uint32_t bit
) {
	return INDICES_PER_CHUNK * ccount + 2049 +
	       chunk * 512 +
	       idx * 32 +
	       bit;
}

static index_t indexWSha256(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 1 +
	       idx * 32 + 
	       bit;
}	//2048 w indices

static index_t indexSig0Sha256(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 2049 + 
	       idx * 32 + 
	       bit;
}	//1536 sig0 indices

static index_t indexSig1Sha256(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 3586 +
	       idx * 32 +
	       bit;
}	//1536 sig1 indices

static index_t indexCcSha256(
	uint32_t chunk, uint32_t kind, uint32_t idx, uint32_t bit
) {
	return INDICES_PER_CHUNK * chunk + 5123 +
	       idx * 32 + 
	       bit;
}	//16384 cc indices

static index_t indexEp0Sha256(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 21508 +
	       idx * 32 +
	       bit;
}	//2048 ep0 indices

static index_t indexEp1Sha256(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 23557 +
	       idx * 32 + 
	       bit;
}	//2048 ep1 indices

static index_t indexChSha256(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 25606 +
	       idx * 32 +
	       bit;
}	//2048 ch indices

static index_t indexMajSha256(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 27655 +
	       idx * 32 + 
	       bit;
}	//2048 maj indices

static index_t indexTemp1Sha256(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 29704 +
	       idx * 32 +
	       bit;
}	//2048 temp1 indices

static index_t indexTemp2Sha256(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 31753 +
	       idx * 32 +
	       bit;
}	//2048 temp2 indices

static index_t indexHhSha256(uint32_t chunk, uint32_t type, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 33802 +
	       type * 32 +
       	       bit;	       
}	//256 hh indices

static index_t indexGenericSha256(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 34059 +
	       idx * 32 + 
	       bit;
}	//25440 generic indices

static int fwriteMessageClausesSha256(Sha256Sat shs) {
	return fwriteAssignClauses(
		shs.stream,
		32,
		shs.w[shs.loop],
		shs.message
	);
}

static int fwriteSig0ClausesSha256(Sha256Sat * shs) {
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

static int fwriteSig1ClausesSha256(Sha256Sat * shs) {
	int res = fwriteSigClausesSha(
		shs->stream,
		32,
		shs->sig1,
		shs->generic,
		shs->w[shs->loop - 2],
		17, 19, 10
	);
	if (res < 0) {
		return -1;
	}

	shs->generic = res;

	return 0;

	return 0;
}
static int fwriteWClausesSha256(Sha256Sat * shs) {
	int res = fwriteSumClauses(
		shs->stream,
		32,
		shs->w[shs->loop],
		shs->generic,
		4,
		shs->w[shs->loop - 16],
		shs->sig0,
		shs->w[shs->loop - 7],
		shs->sig1
	);
	if (res < 0) {
		return -1;
	}

	shs->generic = res;

	return 0;
}

static int fwriteEp0ClausesSha256(Sha256Sat shs) {
	return fwriteEpClausesSha(
		shs.stream,
		32,
		shs.ep0,
		shs.cc[4],
		6, 11, 25
	);
}

static int fwriteChClausesSha256(Sha256Sat shs) {
	return fwriteChClausesSha(
		shs.stream,
		32,
		shs.ch,
		shs.cc[4],
		shs.cc[5],
		shs.cc[6]
	);
}

static int fwriteTemp1ClausesSha256(Sha256Sat * shs) {
	int res = fwriteSumClauses(
		shs->stream,
		32,
		shs->temp1,
		shs->generic,
		5,
		shs->cc[7],
		shs->ep0,
		shs->ch,
		shs->k[shs->loop],
		shs->w[shs->loop]
	);
	if (res < 0) {
		return -1;
	}

	shs->generic = res;

	return 0;
}

static int fwriteEp1ClausesSha256(Sha256Sat shs) {
	return fwriteEpClausesSha(
		shs.stream,
		32,
		shs.ep1,
		shs.cc[0],
		2, 13, 22
	);
}

static int fwriteMajClausesSha256(Sha256Sat shs) {
	return fwriteMajClausesSha(
		shs.stream,
		32,
		shs.maj,
		shs.cc[0],
		shs.cc[1],
		shs.cc[2]
	);
}

static int fwriteTemp2ClausesSha256(Sha256Sat * shs) {
	int res = fwriteSumClauses(
		shs->stream,
		32,
		shs->temp1,
		shs->generic,
		2,
		shs->ep1,
		shs->maj
	);
	if (res < 0) {
		return -1;
	}

	shs->generic = res;

	return 0;
}

static int fwriteCcClausesSha256(Sha256Sat * shs) {
	int res = 0;

	for (int i = 7; i >= 0; i--) {
		shs->cc[i] = indexCcSha256(
			shs->chunk, i, shs->loop + 1, 0
		);

		//e = d + temp1
		if (i == 4) {
			res = fwriteSumClauses(
				shs->stream, 
				32,
				shs->cc[i], 
				shs->generic,
				2, 
				shs->cc[i - 1], 
				shs->temp1
			);

			shs->generic = res;
		}
		//a = temp1 + temp2
		else if (i == 0) {
			res = fwriteSumClauses(
				shs->stream, 
				32,
				shs->cc[i], 
				shs->generic,
				2, 
				shs->temp1, 
				shs->temp2
			);
		
			shs->generic = res;
		}
		else {
			res = fwriteAssignClauses(
				shs->stream, 32, shs->cc[i], shs->cc[i - 1]
			);
		}

		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

static int fwriteHhClausesSha256(Sha256Sat * shs) {
	int res = 0;
	index_t hh[8] = { 0 };

	memcpy(hh, shs->hh, sizeof(hh));

	for (int i = 0; i < 8; i++) {
		shs->hh[i] = indexHhSha256(shs->chunk, i, 0);
		
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
	}

	return 0;
}

static int fwriteKAtomsSha256(Sha256Sat shs) {
	int res = 0;

	uint32_t k[64] = { 
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 
		0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 
		0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 
		0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 
		0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 
		0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 
		0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 
		0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 
		0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	};

	for (int i = 0; i < 64; i++) {
		res = fwriteAtoms32(
			shs.stream, shs.k[i], k[i]
		);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

//carries out a SHA-256 or SHA-224 reduction, depending on 'dsize' parameter
static int _sha256sat(
	FILE *stream, size_t msize, const char *digest, size_t dsize
) {
	int res = 0;
	Sha256Sat shs = {
		stream,
		digest,
		dsize,
		0,
		0,
		0,
		0,
		{ 0 },
		{ 0 },
		0,
		0,
		{ 0 },
		0,
		0,
		0,
		0,
		0,
		0,
		{ 0 },
	};

	//function is a specific implementation for SHA-256 and SHA-224
	if (dsize != 256 && dsize != 224) {
		return -1;
	}

	//preprocess SHA-256
	msize = fwritePreprocClausesSha(
		stream, 0, msize, 512
	);
	if (msize == 0) {
		return -1;
	}

	//determine how many chunks there are
	const uint32_t ccount = msize / 512;

	//initialize k indices
	for (int i = 0; i < 64; i++) {
		shs.k[i] = indexKSha256(ccount, i, 0);
	}

	//initialize hh indicies
	for (int i = 0; i < 8; i++) {
		shs.hh[i] = indexHhSha256(shs.chunk, i, 0);
	}

	//write the dimacs file header
	res = fprintf(
		shs.stream, "p cnf %lu %lu",
		INDICES_PER_CHUNK * ccount + msize + 2048,
		CLAUSES_PER_CHUNK * ccount + msize + 2048
	);
	if (res < 0) {
		return -1;
	}

	//write atoms representing round constant values
	res = fwriteKAtomsSha256(shs);
	if (res < 0) {
		return -1;
	}

	//write atoms representing initial hash values
	res = fwriteHhAtomsSha256(shs);
	if (res < 0) {
		return -1;
	}

	//write clauses representing each and every round of SHA-256
	for (shs.chunk = 0; shs.chunk < ccount; shs.chunk++) {
		//a = h0 ... h = h7
		for (int i = 0; i < 8; i++) {
			shs.cc[i] = shs.hh[i];
		}

		for (shs.loop = 0; shs.loop < 64; shs.loop++) {
			//determine all loop based indices
			shs.sig0 = indexSig0Sha256(shs.chunk, shs.loop, 0);
			shs.sig1 = indexSig1Sha256(shs.chunk, shs.loop, 0);
			shs.w[shs.loop] = indexWSha256(shs.chunk, shs.loop, 0);
			shs.ep0 = indexEp0Sha256(shs.chunk, shs.loop, 0);
			shs.ch 	= indexChSha256(shs.chunk, shs.loop, 0);
			shs.temp1 = indexTemp1Sha256(shs.chunk, shs.loop, 0);
			shs.ep1 = indexEp1Sha256(shs.chunk, shs.loop, 0);
			shs.maj = indexMajSha256(shs.chunk, shs.loop, 0);
			shs.temp2 = indexTemp2Sha256(shs.chunk, shs.loop, 0);

			shs.generic = indexGenericSha256(
				shs.chunk, shs.loop, 0
			);

			//break the chunk into sixteen 32 bit words
			if (shs.loop < 16) {
				//determine index of current message word
				shs.message = indexMessageSha256(
					ccount, shs.chunk, shs.loop, 0
				);

				if (fwriteMessageClausesSha256(shs) < 0) {
				       return -1;	
				}
			}
		       	//key extension	
			else {
				if (
					(fwriteSig0ClausesSha256(&shs) < 0) ||
					(fwriteSig1ClausesSha256(&shs) < 0) ||
					(fwriteWClausesSha256(&shs) < 0)
				) {
					return -1;
				}
			}

			//compression function
			if (
				(fwriteEp0ClausesSha256(shs) < 0) ||
				(fwriteChClausesSha256(shs) < 0) ||
				(fwriteTemp1ClausesSha256(&shs) < 0) ||
				(fwriteEp1ClausesSha256(shs) < 0) ||
				(fwriteMajClausesSha256(shs) < 0) ||
				(fwriteTemp2ClausesSha256(&shs) < 0) ||
				(fwriteCcClausesSha256(&shs) < 0)
			) {
				return -1;
			}
		}

		//update hash values
		if (fwriteHhClausesSha256(&shs) < 0) {
			return -1;
		}
	}

	//write atoms representing the final digest value
	res = fwriteDigestAtomsSha(shs.stream, shs.hh, digest, dsize);
	if (res < 0) {
		return -1;
	}

	return 0;
}

int sha224sat(FILE * stream, size_t msize, const char * digest) {
	return _sha256sat(stream, msize, digest, 224);
}

int sha256sat(FILE * stream, size_t msize, const char * digest) {
	return _sha256sat(stream, msize, digest, 256);
}
