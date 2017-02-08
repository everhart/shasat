#include "preprocess.h"

size_t preprocessSHA(
	FILE *		stream,
	size_t		msize,
	size_t		csize
) {
	int res = 0,
	    atom = 0;

	const uint64_t osize = msize;
	
	//do not proceed if msize is 0
	if (msize == 0) {
		return 0;
	}

	//append the bit '1' to the message
	msize++;
	res = fprintf(
		stream, "%zd 0\n", msize
	);
	if (res < 0) {
		return 0;
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
			return 0;
		}
 	}

	return msize;
}
