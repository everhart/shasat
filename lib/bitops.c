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
