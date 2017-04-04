#include "./sha1sat.h"

static const uint32_t INDICES_PER_CHUNK = 41286;
static const uint32_t CLAUSES_PER_CHUNK = 0;

typedef struct Sha1Sat {
    size_t      i;
    index_t     gen;
    index_t     msg;
    index_t     k[4];
    index_t     w[80];
    index_t     cc[5];
    index_t     f;
    index_t     g;
    index_t     temp;
    index_t     hh[5];
} Sha1Sat;

static inline index_t index_k(size_t ccount, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * ccount +     //amount of previous indices
	       word * 32 +                      //word index
           bit;	                            //bit index
}   //128 k indices

static inline index_t index_init(size_t ccount, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * ccount + 129 + 
           word + 32 + 
           bit;
}   //160 init indices

static inline index_t index_message(
	size_t ccount, size_t chunk, size_t word, size_t bit
) {
    return INDICES_PER_CHUNK * ccount + 290 + 
           chunk * 512 +
           word * 32 +
           bit;	
}   //arbitrary amount of message indices

static inline index_t index_w(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 1 +
           word * 32 +
           bit;
}   //2560 w indices

static inline index_t index_cc(
    size_t chunk, size_t kind, size_t word, size_t bit
) {
    return INDICES_PER_CHUNK * chunk + 2561 + 
           kind * 2592 + 
           word * 32 +
           bit;
}   //12800 cc indices

static inline index_t index_f(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 15362 +
           word * 32 +
           bit;
}	//2560 f indices

static inline index_t index_g(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 17923 +
           word * 32 +
           bit;
}   //2560 g indices

static inline index_t index_temp(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 20484 +
           word * 32 +
           bit;
}   //2560 temp indices

static index_t index_hh(size_t chunk, size_t kind, size_t bit) {
    return INDICES_PER_CHUNK + chunk + 23045 +
           kind * 32 +
           bit;
}   //160 hh indices

static inline index_t index_generic(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 23206 + 
           word * 32 + 
           bit;
}   //18080 generic indices

static int fwrite_sha1_message_clauses(FILE * stream, Sha1Sat ctx) {
	return fwrite_iff_clauses(
		stream,
		32,
		ctx.w[ctx.i],
		ctx.msg
	);
}

static int fwrite_sha1_w_clauses(FILE * stream, Sha1Sat ctx) {
	int res = 0;
	bool comb[4] = { 0 }, eval = 0;
	atom_t ante[4] = { 0 }, cons = 0;

	for (int i = 0; i < (1 << 4); i++) {
		*comb = next_combination(comb, 4);
		eval = comb[0] ^ comb[1] ^ comb[2] ^ comb[3];

		for (int j = 0; j < 32; j++) {
			ante[0] = sign_atom(ctx.w[ctx.i - 3] + j, comb[0]);
			ante[1] = sign_atom(ctx.w[ctx.i - 8] + j, comb[1]);
			ante[2] = sign_atom(ctx.w[ctx.i - 14] + j, comb[2]);
			ante[3] = sign_atom(ctx.w[ctx.i - 16] + j, comb[3]);
			cons = sign_atom(
                ctx.w[ctx.i] + bit_position_lro(32, j, 1), eval
			);

			res = fwrite_clauses(
				stream, ante, 3, &cons, 1
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
	
	if (shs.loop < 20) {
		res = fwriteChClausesSha(
			shs.stream, 
			32, 
			shs.f,
			shs.cc[1], 
			shs.cc[2], 
			shs.cc[3]
		);
	}
	else if (shs.loop >= 40 && shs.loop < 60) {
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
		shs->cc[i] = indexCcSha1(shs->chunk, i, shs->loop + 1, 0);

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
		shs->hh[i] = indexHhSha1(shs->chunk, i, 0);

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
	int res = 0;

	//determine how many chunks there are
	const uint32_t ccount = (msize + 576) / 512;

	Sha1Sat shs = {
		stream,	
		digest,
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

	//initialize message index
	shs.message = indexMessageSha1(ccount, 0, 0, 0);

	//initialize k indices
	for (int i = 0; i < 4; i++) {
		shs.k[i] = indexKSha1(ccount, i, 0);
	}

	//initialize hh indices
	for (int i = 0; i < 4; i++) {
		shs.hh[i] = indexHhSha1(shs.chunk, i, 0);
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

	//preprocess SHA
	msize = fwritePreprocClausesSha(
		stream, shs.message, msize, 512
	);
	if (msize == 0) {
		return -1;
	}

	//write atoms representing round constant values
	res = fwriteKAtomsSha1(shs);
	if (res < 0) {
		return -1;
	}

	//write atoms representing initial hash values
	res = fwriteHhAtomsSha1(shs);
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
			shs.w[shs.loop] = indexWSha1(shs.chunk, shs.loop, 0);
			shs.f = indexFSha1(shs.chunk, shs.loop, 0);
			shs.g = indexGSha1(shs.chunk, shs.loop, 0);
			shs.temp = indexTempSha1(shs.chunk, shs.loop, 0);

			//break chunk into sixteen 32-bit words
			if (shs.loop < 16) {
				shs.message = indexMessageSha1(
					ccount, shs.chunk, shs.loop, 0
				);

				if (fwriteMessageClausesSha1(shs) < 0) {
					return -1;
				}	
			}
			//key extension	
			else {
				if (fwriteWClausesSha1(shs) < 0) {
					return -1;
				}
			}

			//compression function
			if (
				(fwriteFClausesSha1(shs) < 0) ||
				(fwriteGClausesSha1(shs) < 0) ||
				(fwriteTempClausesSha1(&shs) < 0) ||
				(fwriteCcClausesSha1(&shs) < 0)
			) {
				return -1;
			}
		}
	
		//update hash values
		if (fwriteHhClausesSha1(&shs) < 0) {
			return -1;
		}
	}
	
	res = fwriteDigestAtomsSha(
		shs.stream, shs.hh, shs.digest, 160 
	);
	if (res < 0) {
		return -1;
	}

	return 0;
}
