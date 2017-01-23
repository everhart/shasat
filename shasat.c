#include "shasat.h"

int fwriteAtoms32(
	FILE *		stream,
	const uint32_t	inp[],
	const uint32_t	oup[],
	uint64_t	leng
) {
	int res = 0,
	    atom = 0;

	for (int i = 0; i < leng; i++) {
		for (int j = 0; j < sizeof(uint32_t); j++) {
			atom = oup[i] * 
			       (inp[j] >> i & 1) ? 1 : -1;

			res = fprintf(
				stream, "%d 0\n", atom
			);
			if (res < 0) {
				return -1;
			}
		}
	}

	return 0;
}
