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

int fwriteAtoms64(
	FILE *		stream,
	const uint32_t	inp[],
	const uint64_t	oup[],
	uint64_t	leng
) {
	int res = 0,
	    atom = 0;

	for (int i = 0; i < leng; i++) {
		for (int j = 0; j < sizeof(uint64_t); j++) {
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

int preprocessSHA(
	FILE *		stream,
	size_t		msize,
	size_t		csize
) {
	int res = 0,
	    atom = 0;

	const uint64_t osize = msize;
	
	//append the bit '1' to the message
	msize++;
	res = fprintf(
		stream, "%zd 0\n", msize
	);
	if (res < 0) {
		return -1;
	}

	//append '0' to the message until message length % csize is
	//csize - 64
	while (msize % csize != csize - 64) {
		msize++;
		res = fprintf(
			stream, "%zd 0\n", -msize
		);
		if (res < 0) {
			return -1;
		}
	}

	//append original message length to the message as a 64 bit
	//big-endian integer
	for (int i = 63; i >= 0; i--) {
		atom = msize++ * 
		       (osize >> i & 1) ? 1 : -1;
		
		res = fprintf(
			stream, "%d 0\n", atom
		);
		if (res < 0) {
			return -1;
		}
 	}

	return msize;
}
