#include "./bitops.h"

int word32_bit(uint32_t word, size_t n) {
	return (n < 32) ? (word >> n) & 1 : -1;
}

int word64_bit(uint64_t word, size_t n) {
	return (n < 64) ? (word >> n) & 1 : -1;
}

ssize_t bit_position_rro(size_t wsize, size_t n, size_t rot) {
    if (n >= wsize) {
        return -1;
    }

    if (rot >= wsize) {
        rot %= wsize; 
    }

    return (n >= rot) ? n - rot : wsize - rot + n;
}

ssize_t bit_position_lro(size_t wsize, size_t n, size_t rot) {
    if (n >= wsize) {
        return -1;
    }

    if (rot >= wsize) {
        rot %= wsize;
    }

    return (wsize - n >= rot) ? n + rot : n - (wsize - rot);
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
