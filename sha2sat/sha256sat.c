#include "sha256sat.h"

static const size_t INDICES_PER_CHUNK = 59499;
static const size_t CLAUSES_PER_CHUNK = 848494;

typedef struct Sha256Sat {
    size_t      i;
    size_t      j;
    index_t     gen;
    index_t     msg;
    index_t     k[64];
    index_t     w[64];
    index_t     sig0;
    index_t     sig1;
    index_t     cc[8];
    index_t     ep0;
    index_t     ep1;
    index_t     ch;
    index_t     maj;
    index_t     temp1;
    index_t     temp2;
    index_t     hh[8];  
} Sha256Sat;

static index_t index_sha256_k(size_t ccount, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * ccount + 1 +
           word * 32 +
           bit;
}   //2048 k indices

static index_t index_sha256_init(size_t ccount, size_t kind, size_t bit) {
    return INDICES_PER_CHUNK * ccount + 2049 + 
           kind * 32 +
           bit;
}   //256 init indices

static index_t index_sha256_msg(
    size_t ccount, size_t chunk, size_t word, size_t bit
) {
    return INDICES_PER_CHUNK * ccount + 2306 +
           chunk * 512 + 
           word * 32 +
           bit;
}   //arbitrary msg indices

static index_t index_sha256_w(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 1 +
           word * 32 + 
           bit;
}   //2048 w indices

static index_t index_sha256_sig0(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 2049 + 
           word * 32 + 
           bit;
}   //1536 sig0 indices

static index_t index_sha256_sig1(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 3586 +
           word * 32 +
           bit;
}   //1536 sig1 indices

static index_t index_sha256_cc(
    size_t chunk, size_t kind, size_t word, size_t bit
) {
    return INDICES_PER_CHUNK * chunk + 5123 +
           kind * 2048 +
           word * 32 + 
           bit;
}   //16384 cc indices

static index_t index_sha256_ep0(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 21508 +
           word * 32 +
           bit;
}   //2048 ep0 indices

static index_t index_sha256_ep1(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 23557 +
           word * 32 + 
           bit;
}   //2048 ep1 indices

static index_t index_sha256_ch(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 25606 +
           word * 32 +
           bit;
}   //2048 ch indices

static index_t index_sha256_maj(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 27655 +
           word * 32 + 
           bit;
}   //2048 maj indices

static index_t index_sha256_temp1(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 29704 +
           word * 32 +
           bit;
}   //2048 temp1 indices

static index_t index_sha256_temp2(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 31753 +
           word * 32 +
           bit;
}   //2048 temp2 indices

static index_t index_sha256_hh(size_t chunk, size_t type, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 33802 +
           type * 32 +
               bit;        
}   //256 hh indices

static index_t index_sha256_gen(size_t chunk, size_t word, size_t bit) {
    return INDICES_PER_CHUNK * chunk + 34059 +
           word * 32 + 
           bit;
}   //25440 gen indices

static int fwrite_sha256_msg_clauses(FILE * stream, Sha256Sat ctx) {
    return fwrite_iff_clauses(
        stream,
        32,
        ctx.w[ctx.j],
        ctx.msg
    );
}

static int fwrite_sha256_sig0_clauses(FILE * stream, Sha256Sat * ctx) {
    int res = fwrite_sha_sig_clauses(
        stream,
        32,
        ctx->sig0,
        ctx->gen,
        ctx->w[ctx->j - 15],
        7, 18, 3
    );
    if (res < 0) {
        return -1;
    }

    ctx->gen = res;
    return 0;
}

static int fwrite_sha256_sig1_clauses(FILE * stream, Sha256Sat * ctx) {
    int res = fwrite_sha_sig_clauses(
        stream,
        32,
        ctx->sig1,
        ctx->gen,
        ctx->w[ctx->j - 2],
        17, 19, 10
    );
    if (res < 0) {
        return -1;
    }

    ctx->gen = res;
    return 0;
}
static int fwrite_sha256_w_clauses(FILE * stream, Sha256Sat * ctx) {
    int res = fwrite_sum_clauses(
        stream,
        32,
        ctx->w[ctx->j],
        ctx->gen,
        4,
        ctx->w[ctx->j - 16],
        ctx->sig0,
        ctx->w[ctx->j - 7],
        ctx->sig1
    );
    if (res < 0) {
        return -1;
    }

    ctx->gen = res;
    return 0;
}

static int fwrite_sha256_ep0_clauses(FILE * stream, Sha256Sat ctx) {
    return fwrite_sha_ep_clauses(
        stream,
        32,
        ctx.ep0,
        ctx.cc[4],
        6, 11, 25
    );
}

static int fwrite_sha256_ch_clauses(FILE * stream, Sha256Sat ctx) {
    return fwrite_sha_ch_clauses(
        stream,
        32,
        ctx.ch,
        ctx.cc[4],
        ctx.cc[5],
        ctx.cc[6]
    );
}

static int fwrite_sha256_temp1_clauses(FILE * stream, Sha256Sat * ctx) {
    int res = fwrite_sum_clauses(
        stream,
        32,
        ctx->temp1,
        ctx->gen,
        5,
        ctx->cc[7],
        ctx->ep0,
        ctx->ch,
        ctx->k[ctx->j],
        ctx->w[ctx->j]
    );
    if (res < 0) {
        return -1;
    }

    ctx->gen = res;
    return 0;
}

static int fwrite_sha256_ep1_clauses(FILE * stream, Sha256Sat ctx) {
    return fwrite_sha_ep_clauses(
        stream,
        32,
        ctx.ep1,
        ctx.cc[0],
        2, 13, 22
    );
}

static int fwrite_sha256_maj_clauses(FILE * stream, Sha256Sat ctx) {
    return fwrite_sha_maj_clauses(
        stream,
        32,
        ctx.maj,
        ctx.cc[0],
        ctx.cc[1],
        ctx.cc[2]
    );
}

static int fwrite_sha256_temp2_clauses(FILE * stream, Sha256Sat * ctx) {
    int res = fwrite_sum_clauses(
        stream,
        32,
        ctx->temp1,
        ctx->gen,
        2,
        ctx->ep1,
        ctx->maj
    );
    if (res < 0) {
        return -1;
    }

    ctx->gen = res;
    return 0;
}

static int fwrite_sha256_cc_clauses(FILE * stream, Sha256Sat * ctx) {
    int res = 0;

    for (int i = 7; i >= 0; i--) {
        ctx->cc[i] = index_sha256_cc(ctx->i, i, ctx->j + 1, 0);

        switch (i) {
        //e = d + temp1
        case 4:
            res = fwrite_sum_clauses(
                stream,
                32,
                ctx->cc[i], 
                ctx->gen,
                2,
                ctx->cc[i - 1],
                ctx->temp1
            );
            ctx->gen = res;
            break;
        //a = temp1 + temp2
        case 0:
            res = fwrite_sum_clauses(
                stream,
                32,
                ctx->cc[i],
                ctx->gen,
                2,
                ctx->temp1,
                ctx->temp2
            );
            ctx->gen = res;
            break;
        default:
            res = fwrite_iff_clauses(stream, 32, ctx->cc[i], ctx->cc[i - 1]);
            break;
        }
        if (res < 0) {
            return -1;
        }
    }

    return 0;
}

static int fwrite_sha256_hh_clauses(FILE * stream, Sha256Sat * ctx) {
    int res = 0;
    index_t hh[8] = { 0 };
    memcpy(hh, ctx->hh, sizeof(hh));

    for (int i = 0; i < 8; i++) {
        ctx->hh[i] = index_sha256_hh(ctx->i, i, 0);
        
        res = fwrite_sum_clauses(
            stream,
            32,
            ctx->hh[i],
            ctx->gen,
            2,
            hh[i],
            ctx->cc[i]
        );
        if (res < 0) {
            return -1;
        }
    }

    return 0;
}

static int fwrite_sha256_k_atoms(FILE * stream, Sha256Sat ctx) {
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
        res = fwrite_word32_atoms(
            stream, ctx.k[i], k[i]
        );
        if (res < 0) {
            return -1;
        }
    }

    return 0;
}

static int fwrite_sha256_init_atoms(FILE * stream, Sha256Sat ctx, size_t dsize) {
    int res = 0;
    int hh[8] = { 0 };

    //initial hash values are dependant on digest size
    if (dsize == 224) {
        int tmp[8] = {
            0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 
            0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4
        };
        memcpy(hh, tmp, sizeof(hh));
    }
    else if (dsize == 256) {
        int tmp[8] = {
            0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
            0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
        };
        memcpy(hh, tmp, sizeof(hh));
    }
    else {
        return -1;
    }

    for (int i = 0; i < 8; i++) {
        res = fwrite_word32_atoms(
            stream, ctx.hh[i], hh[i]
        );

        if (res < 0) {
            return -1;
        }
    }

    return 0;
}

//carrieso out a sha256 or sha224 reduction, depending on 'dsize' parameter
static int fwrite_sha256_clauses(
    FILE *stream, size_t msize, const char * digest, size_t dsize
) {
    if (msize >= 448 || (dsize != 64 && dsize != 56)) {
        return -1;
    }

    int res = 0;
    const size_t ccount = 0;
    Sha256Sat ctx;

    res = fwrite_sha_preprocessing_atoms(
        stream, index_sha256_msg(ccount, 0, 0, 0), msize, 512
    );
    if (res < 0) {
        return -1;
    }

    //determine round constant indices
    for (int i = 0; i < 64; i++) {
        ctx.k[i] = index_sha256_k(ccount, i, 0);
    }

    //determine initial hash indices
    for (int i = 0; i < 8; i++) {
        ctx.hh[i] = index_sha256_init(ccount, i, 0);
    }

    //for each chunk
    for (ctx.i = 0; ctx.i < ccount; ctx.i++) {
        //a = h0 ... h = h7
        for (int i = 0; i < 8; i++) {
            ctx.cc[i] = ctx.hh[i];
        }
    
        for (ctx.j = 0; ctx.j < 64; ctx.j++) {
            //determine all loop based indices
            ctx.sig0 = index_sha256_sig0(ctx.i, ctx.j, 0);
            ctx.sig1 = index_sha256_sig1(ctx.i, ctx.j, 0);
            ctx.w[ctx.j] = index_sha256_w(ctx.i, ctx.j, 0);
            ctx.ep0 = index_sha256_ep0(ctx.i, ctx.j, 0);
            ctx.ep1 = index_sha256_ep1(ctx.i, ctx.j, 0);
            ctx.ch  = index_sha256_ch(ctx.i, ctx.j, 0);
            ctx.maj = index_sha256_maj(ctx.i, ctx.j, 0);
            ctx.temp1 = index_sha256_temp1(ctx.i, ctx.j, 0);
            ctx.temp2 = index_sha256_temp2(ctx.i, ctx.j, 0);
            ctx.gen = index_sha256_gen(ctx.i, ctx.j, 0);

            //break the chunk into sixteen 32 bit words
            if (ctx.j < 16) {
                //determine index of current message word
                ctx.msg = index_sha256_msg(ccount, ctx.i, ctx.j, 0);

                //w[0 .. 16] = msg[n .. n + 16]
                res = fwrite_sha256_msg_clauses(stream, ctx);
                if (res < 0) {
                    return res;
                }
            }
            //key extension
            if (ctx.j < 48) {
                res = fwrite_sha256_sig0_clauses(stream, &ctx);
                if (res < 0) {
                    return -1;
                }

                res = fwrite_sha256_sig1_clauses(stream, &ctx);
                if (res < 0) {
                    return -1;
                }

                res = fwrite_sha256_w_clauses(stream, &ctx);
                if (res < 0) {
                    return -1;
                }
            }

            //compression function
            res = fwrite_sha256_ep0_clauses(stream, ctx);
            if (res < 0) {
                return -1;
            }

            res = fwrite_sha256_ep1_clauses(stream, ctx);
            if (res < 0) {
                return -1;
            }

            res = fwrite_sha256_ch_clauses(stream, ctx);
            if (res < 0) {
                return -1;
            }

            res = fwrite_sha256_maj_clauses(stream, ctx);
            if (res < 0) {
                return -1;
            }
        
            res = fwrite_sha256_temp1_clauses(stream, &ctx);
            if (res < 0) {
                return -1;
            }
        
            res = fwrite_sha256_temp2_clauses(stream, &ctx);
            if (res < 0) {
                return -1;
            }
        
            res = fwrite_sha256_cc_clauses(stream, &ctx);
            if (res < 0) {
                return -1;
            }
        }

        //update hash values
        res = fwrite_sha256_hh_clauses(stream, &ctx);
        if (res < 0) {
            return -1;
        }
    }

    //assign round constant atoms
    res = fwrite_sha256_k_atoms(stream, ctx);
    if (res < 0) {
        return -1;
    }

    //assign initial hash atoms
    res = fwrite_sha256_init_atoms(stream, ctx, dsize);
    if (res < 0) {
        return -1;
    }

    //assign digest atoms
    res = fwrite_sha_digest_word32_atoms(stream, ctx.hh, digest, dsize);
    if (res < 0) {
        return -1;
    }

    return 0;
}

int sha224sat(FILE * stream, size_t msize, const char * digest) {
    return fwrite_sha256_clauses(stream, msize, digest, 56);
}

int sha256sat(FILE * stream, size_t msize, const char * digest) {
    return fwrite_sha256_clauses(stream, msize, digest, 64);
}
