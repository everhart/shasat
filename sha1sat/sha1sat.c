#include "sha1sat.h"

#define SHA1_ATOMS_PER_CHUNK 0

static uint32_t indexKBitSha1(
	uint32_t ccount, uint32_t idx, uint32_t bit
) {
	return 0;
}

static uint32_t indexMessageBitSha1(
	uint32_t ccount, uint32_t idx, uint32_t bit
) {
	return 0;
}

static uint32_t indexMsBitSha1(
	uint32_t chunk, uint32_t idx, uint32_t bit
) {
	return 0;
}

static uint32_t indexWvBitSha1(
	uint32_t kind, uint32_t chunk, uint32_t idx, uint32_t bit
) {
	return 0;
}

static uint32_t indexFBitSha1(
	uint32_t chunk, uint32_t idx, uint32_t bit
) {
	return 0;
}

static uint32_t indexTempBitSha1(
	uint32_t chunk, uint32_t idx, uint32_t bit
) {
	return 0;
}

static uint32_t indexHhBitSha1(
	uint32_t kind, uint32_t chunk, uint32_t bit
) {
	return 0;
}

int sha1sat(
	FILE * 		stream, 
	size_t 		msize, 
	const char * 	digest
) {
	int res = 0;

	return 0;
}
