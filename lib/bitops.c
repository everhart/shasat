#include "./bitops.h"

int bitValue32(uint32_t word, uint32_t pos) {
	if (pos >= 32) {
		return -1;
	}

	return (word >> pos) & 1;
}

int bitValue64(uint64_t word, uint32_t pos) {
	if (pos >= 64) {
		return -1;
	}

	return (word >> pos) & 1;
}

int bitPositionRsh(uint32_t wsize, uint32_t pos, uint32_t rsh) {
	if (pos >= wsize || pos < rsh) {
		return -1;
	}

	return pos - rsh;
}

int bitPositionLsh(uint32_t wsize, uint32_t pos, uint32_t lsh) {
	if (pos + lsh >= wsize) {
		return -1;
	}

	return pos + lsh;
}

uint32_t bitPositionRro(uint32_t wsize, uint32_t pos, uint32_t rro) {
	if (rro > wsize) {
		rro = wsize - rro;
	}

	return (pos > rro) ? pos - rro : wsize - rro + pos;
}	
