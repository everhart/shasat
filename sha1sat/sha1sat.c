#include "sha1sat.h"

uint32_t indexHashBitSHA1(
	uint32_t 	chunk,
	uint32_t	word,
	uint32_t 	bit
) {
	return (chunk * SHA1_ATOMS_PER_CHUNK) + 
	       (word * 32) +
	       bit +
	       1;	//160 hash bits
}

uint32_t indexMessageScheduleBitSHA1(
	uint32_t	chunk,
	uint32_t	word,
	uint32_t	bit
) {
	return (chunk * SHA1_ATOMS_PER_CHUNK) +
	       (word * 32) +
	       bit +
	       161;	//2560 message schedule bits
}

uint32_t indexWorkingVarBitSHA1(
	uint32_t 	chunk,
	uint32_t 	word,
	uint32_t	bit
) {
	return (chunk * SHA1_ATOMS_PER_CHUNK) +
	       (word * 32) +
	       bit +
	       2721;	//15520 working variable bits
}

uint32_t indexBufferBitSHA1(
	uint32_t	chunk,
	uint32_t	word,
	uint32_t	bit
) {
	return (chunk * SHA1_ATOMS_PER_CHUNK) +
	       18241 +
	       (word * 32) +
	       bit;	//16896 buffer bits
}
