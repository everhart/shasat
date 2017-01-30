#include "./routines.h"

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

int bitPositionRro(size_t wsize, uint32_t pos, uint32_t rro) {
	if (pos >= wsize) {
		return -1;
	}
	
	if (rro > wsize) {
		rro -= wsize;
	}

	return (pos >= rro) ? 
		pos - rro : wsize - rro + pos;
}

int bitPositionLro(size_t wsize, uint32_t pos, uint32_t lro) {
	if (pos >= wsize) {
		return -1;
	}

	if (lro > wsize) {
		lro -= wsize;
	}

	return ((wsize - pos) >= lro) ? 
		pos + lro : pos - (wsize - lro);
}

inline int signAtom(int atom, bool state) {
	return (state) ? atom : -atom;
}

bool * nextPermutation(bool * arr, size_t leng) {
	bool tmp = 0, 
	     crr = 0;	

	for (int i = leng - 1; i >= 0 && crr > 0; i--) {
		tmp = arr[i] + crr;
		crr = tmp >> 1;
		arr[i] = tmp & 1;
	}	

	return arr;
}

int fwriteClauses(
	FILE * 		stream, 
	int * 		ante, 
	size_t 		aleng, 
	int * 		cons, 
	size_t 		cleng
) {
	int res = 0; 

	for (int i = 0; i < cleng; i++) {
		for (int j = 0; j < aleng; j++) {
			res = fprintf(stream, "%d ", -ante[j]);
			if (res < 0) {
				return -1;
			}
		}

		res = fprintf(stream, "%d 0\n", cons[i]);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

