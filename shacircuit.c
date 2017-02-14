#include "shacircuit.h"

size_t fwritePreprocClausesSha(
	FILE *		stream,
	index_t		M,
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
	res = fwriteAtom(stream, msize + M);
	if (res < 0) {
		return 0;
	}

	//append '0' to the message until message length % csize is
	//csize - 64
	while (msize % csize != csize - 64) {
		msize++;
		res = fwriteAtom(stream, -(msize + M));
		if (res < 0) {
			return -1;
		}
	}

	//append original message length to the message as a 64 bit
	//big-endian integer 
	//NOTE: cannot use fwriteAtoms64() for this
	for (int i = 63; i >= 0; i--) {
		msize++;
		res = fwriteAtom(
			stream,
			signAtom(msize + M, bitVal64(osize, i))
		);
		if (res < 0) {
			return 0;
		}
 	}

	return msize;
}
