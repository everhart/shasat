#include "sha256sat.h"

static const uint32_t INDICES_PER_CHUNK = 59499;
static const uint32_t CLAUSES_PER_CHUNK = 0;

typedef struct Sha256Sat {
	FILE *		stream;
	char *		digest;	
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

static index_t indexK(uint32_t ccount, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * ccount +
	       idx * 32 +
	       bit;
}

static index_t indexMessage(
	uint32_t ccount, uint32_t chunk, uint32_t idx, uint32_t bit
) {
	return INDICES_PER_CHUNK * ccount + 2049 +
	       chunk * 512 +
	       idx * 32 +
	       bit;
}

static index_t indexW(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 1 +
	       idx * 32 + 
	       bit;
}	//2048 w indices

static index_t indexSig0(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 2049 + 
	       idx * 32 + 
	       bit;
}	//1536 sig0 indices

static index_t indexSig1(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 3586 +
	       idx * 32 +
	       bit;
}	//1536 sig1 indices

static index_t indexCc(
	uint32_t chunk, uint32_t kind, uint32_t idx, uint32_t bit
) {
	return INDICES_PER_CHUNK * chunk + 5123 +
	       idx * 32 + 
	       bit;
}	//16384 cc indices

static index_t indexEp0(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 21508 +
	       idx * 32 +
	       bit;
}	//2048 ep0 indices

static index_t indexEp1(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 23557 +
	       idx * 32 + 
	       bit;
}	//2048 ep1 indices

static index_t indexCh(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 25606 +
	       idx * 32 +
	       bit;
}	//2048 ch indices

static index_t indexMaj(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 27655 +
	       idx * 32 + 
	       bit;
}	//2048 maj indices

static index_t indexTemp1(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 29704 +
	       idx * 32 +
	       bit;
}	//2048 temp1 indices

static index_t indexTemp2(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 31753 +
	       idx * 32 +
	       bit;
}	//2048 temp2 indices

static index_t indexHh(uint32_t chunk, uint32_t type, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 33802 +
	       type * 32 +
       	       bit;	       
}	//256 hh indices

static index_t indexGeneric(uint32_t chunk, uint32_t idx, uint32_t bit) {
	return INDICES_PER_CHUNK * chunk + 34059 +
	       idx * 32 + 
	       bit;
}	//25440 generic indices

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
