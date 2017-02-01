#include "./sha1sat.h"

static const uint32_t INDICES_PER_CHUNK = 38733;

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
static index_t indexH0(uint32_t chunk, uint32_t bit);
static index_t indexH1(uint32_t chunk, uint32_t bit);
static index_t indexH2(uint32_t chunk, uint32_t bit);
static index_t indexH3(uint32_t chunk, uint32_t bit);
static index_t indexH4(uint32_t chunk, uint32_t bit);
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

static index_t indexF(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 15366 +
	       idx * 32 +
	       bit;
}	//2560 f indices

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

int sha1sat(FILE * stream, size_t msize, const char * digest);

