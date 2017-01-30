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

static int fwriteMsClausesSha1(
	FILE *stream, uint32_t ms[80], uint32_t idx
) {
	int res = 0;
	bool perm[4] = { 0 };

	for (int i = 0; i < (1 << 4); i++) {
		*perm = nextPermutation(perm, 4);

		for (int i = 0; i < 32; i++) {
			int ante[4] = {
				signAtom(ms[idx - 3], perm[0]),
				signAtom(ms[idx - 8], perm[1]),
				signAtom(ms[idx - 14], perm[2]),
				signAtom(ms[idx - 16], perm[3])
			};

			int cons = signAtom(
				ms[idx], 
				perm[0] ^ perm[1] ^ perm[2] ^ perm[3]
			);

			res = fwriteClauses(stream, ante, 4, &cons, 1);
			if (res < 0) {
				return -1;
			}
		}
	}

	return 0;
}


static int fwriteFClausesSha1(
	FILE * stream, uint32_t wv[5], uint32_t f, uint32_t idx
) {
	int res = 0;
	bool perm[3] = { 0 };

	for (int i = 0; i < (1 << 3); i++) {
		*perm = nextPermutation(perm, 3);

		for (int i = 0; i < 32; i++) {
			int ante[3] = {
				signAtom(wv[1], perm[0]),
				signAtom(wv[2], perm[1]),
				signAtom(wv[3], perm[2])
			};

			int cons = 0;

			if (idx > 0 && idx < 20) {
				cons = signAtom(
					f,
					(perm[0] & perm[1]) |
					(!perm[0] & perm[2])
				);
			}
			else if (idx > 39 && idx < 60) {
				cons = signAtom(
					f,
					(perm[0] & perm[1]) |
					(perm[0] & perm[1]) |
					(perm[1] & perm[2])
				);
			}
			else {
				cons = signAtom(
					f,
					(perm[0] ^ perm[1] ^ perm[2])
				);
			}

			res = fwriteClauses(stream, ante, 3, &cons, 1);
			if (res < 0) {
				return -1;
			}
		}
	}

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
