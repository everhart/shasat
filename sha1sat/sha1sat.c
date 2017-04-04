#include "./sha1sat.h"

static const uint32_t INDICES_PER_CHUNK = 41286;
static const uint32_t CLAUSES_PER_CHUNK = 0;

typedef struct Sha1Sat {
    size_t      i;
    size_t      j;
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

static inline index_t index_sha1_k(size_t ccount, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * ccount +     //amount of previous indices
	       word * 32 +                      //word index
           bit;	                            //bit index
}   //128 k indices

static inline index_t index_sha1_init(size_t ccount, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * ccount + 129 + 
           word + 32 + 
           bit;
}   //160 init indices

static inline index_t index_sha1_message(
	size_t ccount, size_t chunk, size_t word, size_t bit
) {
    return INDICES_PER_CHUNK * ccount + 290 + 
           chunk * 512 +
           word * 32 +
           bit;	
}   //arbitrary amount of message indices

static inline index_t index_sha1_w(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 1 +
           word * 32 +
           bit;
}   //2560 w indices

static inline index_t index_sha1_cc(
    size_t chunk, size_t kind, size_t word, size_t bit
) {
    return INDICES_PER_CHUNK * chunk + 2561 + 
           kind * 2592 + 
           word * 32 +
           bit;
}   //12800 cc indices

static inline index_t index_sha1_f(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 15362 +
           word * 32 +
           bit;
}	//2560 f indices

static inline index_t index_sha1_g(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 17923 +
           word * 32 +
           bit;
}   //2560 g indices

static inline index_t index_sha1_temp(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 20484 +
           word * 32 +
           bit;
}   //2560 temp indices

static index_t index_sha1_hh(size_t chunk, size_t kind, size_t bit) {
    return INDICES_PER_CHUNK + chunk + 23045 +
           kind * 32 +
           bit;
}   //160 hh indices

static inline index_t index_sha1_generic(
    size_t chunk, size_t word, size_t bit
) {
    return INDICES_PER_CHUNK * chunk + 23206 + 
           word * 32 + 
           bit;
}   //18080 generic indices

static int fwrite_sha1_message_clauses(FILE * stream, Sha1Sat ctx) {
	return fwrite_iff_clauses(
		stream,
		32,
		ctx.w[ctx.j],
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
			ante[0] = sign_atom(ctx.w[ctx.j - 3] + j, comb[0]);
			ante[1] = sign_atom(ctx.w[ctx.j - 8] + j, comb[1]);
			ante[2] = sign_atom(ctx.w[ctx.j - 14] + j, comb[2]);
			ante[3] = sign_atom(ctx.w[ctx.j - 16] + j, comb[3]);
			cons = sign_atom(
                ctx.w[ctx.j] + bit_position_lro(32, j, 1), eval
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

static int fwrite_sha1_f_clauses(FILE * stream, Sha1Sat ctx) {
    int res = 0;

    if (ctx.j < 20) {
        res = fwrite_sha_ch_clauses(
            stream, 32, ctx.f, ctx.cc[1], ctx.cc[2], ctx.cc[3]
        );
    }
    else if (ctx.j >= 40 && ctx.j < 60) {
        res = fwrite_sha_maj_clauses(
            stream, 32, ctx.f, ctx.cc[1], ctx.cc[2], ctx.cc[3]
        );
	}
    else {
        res = fwrite_sha_par_clauses(
            stream, 32, ctx.f, ctx.cc[1], ctx.cc[2], ctx.cc[3]
		);
	}

	return res;
}

static int fwrite_sha1_g_clauses(FILE * stream, Sha1Sat ctx) {
	return fwrite_lro_clauses(stream, 32, ctx.cc[0], ctx.g, 5);
}

static int fwrite_sha1_temp_clauses(FILE * stream, Sha1Sat * ctx) {
	int res = fwrite_sum_clauses(
		stream,
		32,
		ctx->temp,
		ctx->gen,
		5,
		ctx->cc[4],
		ctx->f,
		ctx->g,
		ctx->k[ctx->j / 20],
		ctx->w[ctx->j]
	);
	if (res < 0) {
		return -1;
	}

	ctx->gen = res;

	return 0;
}

static int fwrite_sha1_cc_clauses(FILE * stream, Sha1Sat * ctx) {
	int res = 0;

	for (int i = 4; i >= 0; i--) {
		ctx->cc[i] = index_sha1_cc(ctx->i, i, ctx->j + 1, 0);

		if (i == 2) {
			res = fwrite_lro_clauses(
				stream, 32, ctx->cc[i - 1], ctx->cc[i], 30
			);
		} 
		else {
			res = fwrite_iff_clauses(
				stream, 32, ctx->cc[i - 1], ctx->cc[i]
			);
		}

		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

static int fwrite_sha1_hh_clauses(FILE * stream, Sha1Sat * ctx) {
	int res = 0;
	index_t hh[5] = { 0 };

	memcpy(hh, ctx->hh, sizeof(hh));

	for (int i = 0; i < 5; i++) {
		ctx->hh[i] = index_sha1_hh(ctx->i, i, 0);

		res = fwrite_sum_clauses(
			stream, 32, ctx->hh[i], ctx->gen, 2, hh[i], ctx->cc[i]
		);
		if (res < 0) {
			return -1;
		}

		ctx->gen = res;
	}

	return 0;
}

static int fwrite_sha1_k_atoms(FILE * stream, Sha1Sat ctx) {
	int res = 0;
	int k[4] = {
		0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6
	};

	for (int i = 0; i < 4; i++) {
		res = fwrite_word32_atoms(stream, k[i], ctx.k[i]);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

static int fwrite_sha1_init_atoms(FILE * stream, Sha1Sat ctx) {
	int res = 0;
	int hh[5] = {
		0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476,	0xC3D2E1F0
	};

	for (int i = 0; i < 4; i++) {
		res = fwrite_word32_atoms(stream, hh[i], ctx.hh[i]);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

int sha1sat(FILE * stream, size_t msize, const char * digest) {
    int res = 0;
    const uint32_t ccount = (msize + 576) / 512;
    Sha1Sat ctx = {
        0,
        0,
        0,
        0,
        { 0 },		
        { 0 },
        { 0 },
        0,
        0,
        0,
        { 0 }
    };

    //write the dimacs file header
	res = fprintf(
		stream, "p cnf %lu %lu \n",
		INDICES_PER_CHUNK * ccount + 290 + msize,
		CLAUSES_PER_CHUNK * ccount + 290 + msize
	);
    if (res < 0) {
		return -1;
	}

    //preprocessing
    res = fwrite_sha_preprocessing_atoms(
        stream, index_sha1_message(ccount, 0, 0, 0), msize, 512
    );
    if (res < 0) {
        return -1;
    }

    //determine round constant indices
    for (int i = 0; i < 4; i++) {
        ctx.k[i] = index_sha1_k(ccount, i, 0);
    }

    //determine initial hash indices
    for (int i = 0; i < 5; i++) {
        ctx.hh[i] = index_sha1_init(ccount, i, 0);
    }

    //assign round constant atoms
    res = fwrite_sha1_k_atoms(stream, ctx);
    if (res < 0) {
        return -1;
    }

    //assign initial hash atoms
    res = fwrite_sha1_init_atoms(stream, ctx);
    if (res < 0) {
        return -1;
    }

    //for each chunk
    for (ctx.i = 0; ctx.i < ccount; ctx.i++) {
        //a = h0 .. e = h4
        for (int j = 0; j < 5; j++) {
            ctx.cc[j] = ctx.hh[j];
        }

        for (ctx.j = 0; ctx.j < 80; ctx.j++) {
            //determine all loop based indices
            ctx.w[ctx.j] = index_sha1_w(ctx.i, ctx.j, 0);
            ctx.f = index_sha1_f(ctx.i, ctx.j, 0);
            ctx.g = index_sha1_g(ctx.i, ctx.j, 0);
            ctx.temp = index_sha1_temp(ctx.i, ctx.j, 0);
            ctx.gen = index_sha1_generic(ctx.i, ctx.j, 0);

            //break the chunk into sixteen 32 bit words
            if (ctx.j < 16) {
                //determine message index
                ctx.msg = index_sha1_message(ccount, ctx.i, ctx.j, 0);

                res = fwrite_sha1_message_clauses(stream, ctx);
                if (res < 0) {
                    return -1;
                }
            }
            //key extension
            else {
                res = fwrite_sha1_w_clauses(stream, ctx);
                if (res < 0) {
                    return -1;
                }
            }

            //compression function
            res = fwrite_sha1_f_clauses(stream, ctx);
            if (res < 0) {
                return -1;
            }

            res = fwrite_sha1_g_clauses(stream, ctx);
            if (res < 0) {
                return -1;
            }

            res = fwrite_sha1_temp_clauses(stream, &ctx);
            if (res < 0) {
                return -1;
            }

            res = fwrite_sha1_cc_clauses(stream, &ctx);
            if (res < 0) {
                return -1;
            }
        }

        //update hash values
        res = fwrite_sha1_hh_clauses(stream, &ctx);
        if (res < 0) {
            return res;
        }
    }

    //assign atoms representing the resultant digest
    res = fwrite_sha_digest_word32_atoms(stream, ctx.hh, digest, 160);
    if (res < 0) {
        return 0;
    }

    return 0;
}
