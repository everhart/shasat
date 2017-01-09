#include "sha1sat.h"

uint32_t indexRoundConstBitSHA1(
	uint32_t	word,
	uint32_t	bit
) {
	return (word * 32) + bit;
}

uint32_t indexHashBitSHA1(
	uint32_t 	chunk,
	uint32_t	word,
	uint32_t 	bit
) {
	return (chunk * SHA1_ATOMS_PER_CHUNK) + 
	       129 + 
	       (word * 32) +
	       bit;
}

uint32_t indexMessageScheduleBitSHA1(
	uint32_t	chunk,
	uint32_t	word,
	uint32_t	bit
) {
	return (chunk * SHA1_ATOMS_PER_CHUNK) +
	       290 +
	       (word * 32) +
	       bit;
}

uint32_t indexWorkingVarBitSHA1(
	uint32_t 	chunk,
	uint32_t 	word,
	uint32_t	bit
) {
	return (chunk * SHA1_ATOMS_PER_CHUNK) +
	       2851 +
	       (word * 32) +
	       bit;
}

uint32_t indexBufferBitSHA1(
	uint32_t	chunk,
	uint32_t	word,
	uint32_t	bit
) {
	return (chunk * SHA1_ATOMS_PER_CHUNK) +
	       3236 +
	       (word * 32) +
	       bit;
}
