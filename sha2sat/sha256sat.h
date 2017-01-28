#ifndef SHA256SAT_H
#define SHA256SAT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../shasat.h"

#define ATOMS_PER_CHUNK_SHA256 0

uint32_t indexKBitSha256(
	uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexWBitSha256(
	uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexWvBitSha256(
	uint32_t type, uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexHhBitSha256(
	uint32_t type, uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexEp0BitSha256(
	uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexEp1BitSha256(
	uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexSig0BitSha256(
	uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexSig1BitSha256(
	uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexChBitSha256(
	uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexMajBitSha256(
	uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexTemp1BitSha256(
	uint32_t chunk, uint32_t word, uint32_t bit
);
uint32_t indexTemp2BitSha256(
	uint32_t chunk, uint32_t word, uint32_t bit
);

#endif
