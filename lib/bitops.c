#include "./bitops.h"

int word32_bit(uint32_t word, size_t n) {
	return (n < 32) ? (word >> n) & 1 : -1;
}

int word64_bit(uint64_t word, size_t n) {
	return (n < 64) ? (word >> n) & 1 : -1;
}

int bitPosRro(size_t wsize, uint32_t pos, uint32_t rro) {
	if (pos >= wsize) {
		return -1;
	}
	
	if (rro > wsize) {
		rro -= wsize;
	}

	return (pos >= rro) ? 
		pos - rro : wsize - rro + pos;
}

int bitPosLro(size_t wsize, uint32_t pos, uint32_t lro) {
	if (pos >= wsize) {
		return -1;
	}

	if (lro > wsize) {
		lro -= wsize;
	}

	return ((wsize - pos) >= lro) ? 
		pos + lro : pos - (wsize - lro);
}

bool * nextCombination(bool * arr, size_t leng) {
	bool tmp = 0, 
	     crr = 0;	

	for (int i = leng - 1; i >= 0 && crr > 0; i--) {
		tmp = arr[i] + crr;
		crr = tmp >> 1;
		arr[i] = tmp & 1;
	}	

	return arr;
}
