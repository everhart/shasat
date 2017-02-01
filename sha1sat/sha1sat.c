#include "./sha1sat.h"

static const uint32_t INDICES_PER_CHUNK = 0;

typedef struct SHA1SAT {
	FILE *	stream;
	atom_t	chunk,
		loop,
		generic,
		k[4],
		w[80],
		a,
		b,
		c,
		d,
		e,
		f,
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
static index_t indexF(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexTemp(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexH0(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexH1(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexH2(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexH3(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexH4(uint32_t chunk, uint32_t idx, uint32_t bit);
static index_t indexGeneric(uint32_t chunk, uint32_t idx, uint32_t bit);

static int fwriteWClauses(SHA1SAT sha1sat);
static int fwriteFClauses(SHA1SAT sha1sat);
static int fwriteTempClauses(SHA1SAT sha1sat);
static int fwriteAClauses(SHA1SAT sha1sat);
static int fwriteBClauses(SHA1SAT sha1sat);
static int fwriteCClauses(SHA1SAT sha1sat);
static int fwriteDClauses(SHA1SAT sha1sat);
static int fwriteEClauses(SHA1SAT sha1sat);
static int fwriteH0Clauses(SHA1SAT sha1sat);
static int fwriteH1Clauses(SHA1SAT sha1sat);
static int fwriteH2Clauses(SHA1SAT sha1sat);
static int fwriteH3Clauses(SHA1SAT sha1sat);
static int fwriteH4Clauses(SHA1SAT sha1sat);

static index_t indexK(uint32_t ccount, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * ccount +
	       idx * 32 + 
	       bit;
}

int sha1sat(FILE * stream, size_t msize, const char * digest);

