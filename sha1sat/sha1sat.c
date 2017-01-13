#include "sha1sat.h"

int w[80]; 

uint32_t indexHashBitSHA1(
	uint32_t 	chunk,
	uint32_t	word,
	uint32_t 	bit
) {
	return 0;
}

uint32_t indexMessageScheduleBitSHA1(
	uint32_t	chunk,
	uint32_t	word,
	uint32_t	bit
) {
	return 0;
}

uint32_t indexWorkingVarBitSHA1(
	uint32_t 	chunk,
	uint32_t 	word,
	uint32_t	bit
) {
	return 0;
}

uint32_t indexGenericBitSHA1(
	uint32_t	chunk,
	uint32_t	word,
	uint32_t	bit
) {
	return 0;
}

uint32_t indexRoundConstBitSHA1(
	size_t 		msize,
	uint32_t	bit
) {
	return 0;
}

uint32_t indexMessageBitSHA1(
	size_t		msize,
	uint32_t	bit
) {
	return 0;
}

int fwriteWordExtensionLogic(
	FILE *		stream,
	size_t		wsize,
	uint32_t *	msa,
	uint32_t	idx
) {
	int res = 0;
	int clause[5] = { 0, 0, 0, 0, 0};

	//for each bit	
	for (int i = 0; i < wsize; i++) {
		clause[0] = msa[idx-3] + i;
		clause[1] = msa[idx-8] + i;
		clause[2] = msa[idx-14] + i;
		clause[3] = msa[idx-16] + i;

		//simple way of accounting for w[i] = w[i] lro 1
		clause[4] = (i == 31) ? msa[idx] : msa[idx] + i;

		//permutation loop
		for (int j = 0; j < 16; j++) {
			//determine the current permutation based on each
			//respective modulus evaluation
			clause[0] = -clause[0];
		       	clause[1] = (j % 2) ? clause[1] : -clause[1];
			clause[2] = (j % 3) ? clause[2] : -clause[2];
			clause[3] = (j % 4) ? clause[3] : -clause[3];

			//w[i] =
			//w[i-3] xor w[i-8] xor w[i-14] xor w[i-16]
			clause[0] = (
				clause[0] < 0 !=
				clause[1] < 0 !=
				clause[2] < 0 !=
				clause[3]
			) ? clause[4] : -clause[4];

			res = fprintf(
				stream, "%d %d %d %d %d 0\n",
				clause[0], 
				clause[1], 
				clause[2], 
				clause[3],
				clause[4]
			);
			if (res < 0) {
				return -1;
			}
		}
	}	
	
	return 0;
}

int preprocessSHA1(
	FILE *		stream,
	size_t		msize
) {
	int res = 0;
	uint64_t osize = msize;

	//append the bit '1' to the message
	msize++;
	res = fwriteAtom(
		stream, indexMessageBitSHA1(osize, msize)
	);
	if (res < 0) {
		return res;
	}

	//append '0' to the message until message length % 512 is 448
	while (msize % 512 != 448) {
		msize++;
		res = fwriteAtom(
			stream, -indexMessageBitSHA1(osize, msize)
		);
		if (res < 0) {
			return res;
		}
	}

	//append original message length to the message as a 64 bit 
	//big-endian integer
	for (int i = 63; i >= 0; i--) {
		msize++;
		res = fwriteAtom(
			stream,
			((osize >> i & 1) == 1 ? 1 : -1) *
				indexMessageBitSHA1(osize, msize)
		);
		if (res < 0) {		
			return res;
		}
	}

	return msize;
}

int sha1sat(
	FILE * 		stream, 
	size_t 		msize, 
	const char * 	digest
) {
	return 0;	
}
