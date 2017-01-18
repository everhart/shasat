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

int fwriteMsaClausesSHA1(
	FILE *		stream,
	const uint32_t	inp[],
	uint32_t	oup
) {
	int res = 0,
	    ante[4] = { 0, 0, 0, 0},
	    cons = 0;

	bool tmp = 0,
	     crr = 0,
	     perm[4] = { true, true, true, true };

	//for each permutation of three unique atom states
	for (int i = 0; i < (1 << 3); i++) {
		//modify perm[] to represent the next permutation
		for (int j = 3; j >= 0 && crr > 0; j--) {
			tmp = perm[j] + crr;
			crr = tmp >> 1;
			perm[j] = tmp & 1;
		}
	
		//for each bit
		for (int j = 0; j < 32; j++) {
			ante[0] = (inp[0] + j) * (perm[0] ? 1 : -1);
			ante[1] = (inp[1] + j) * (perm[1] ? 1 : -1);
			ante[2] = (inp[2] + j) * (perm[2] ? 1 : -1);
			ante[3] = (inp[3] + j) * (perm[3] ? 1 : -1);

			//w[i] = 					
			//(w[i-3] xor w[i-8] xor w[i-14] xor w[i-16]) 
			//lro 1
			cons = (i == 31) ? oup : oup + j;
			cons = (perm[0] ^ perm[1] ^ perm[2] ^ perm[3]) ?
				cons : -cons; 

			res = fprintf(
				stream, "%d %d %d %d %d 0\n",
				-ante[0], -ante[1], -ante[2], -ante[3],
				cons
			);
			if (res < 0) {
				return -1;
			}
		}	
		
	}

	return 0;
}

int fwriteFClausesSHA1(
	FILE *			stream,
	const uint32_t		inp[],
	const uint32_t		oup,
	uint32_t		idx
) {
	int res = 0, 
	    ante[3] = { 0, 0, 0 },
	    cons = 0;

	bool tmp = 0, 
	     crr = 0,
	     perm[3] = { true, true, true };

	//for each permutation of four unique atom states
	for (int i = 0; i < (1 << 3); i++) {
		crr = 1;
		//modify perm[] to represent the next permutation
		for (int j = 2; j >= 0 && crr > 0; j--) {
			tmp = perm[j] + crr;
			crr = tmp >> 1;
			perm[j] = tmp & 1;
		}

		//foreach bit
		for (int j = 0; j < 32; j++) {
			ante[0] = (inp[0] + j) * (perm[0] ? 1 : -1);
			ante[1] = (inp[1] + j) * (perm[1] ? 1 : -1);
			ante[2] = (inp[2] + j) * (perm[2] ? 1 : -1);

			cons = oup + j;

			//if 0 ≤ i ≤ 19 then
			if (idx > 0 && idx < 20) {
				//f = (b and c) or ((not b) and d)
				cons = (
					(perm[0] & perm[1]) |
					(!perm[0] & perm[2])
				) ? cons : -cons;
			}
			//else if 40 ≤ i ≤ 59
			else if (idx > 39 && idx < 60) {
				//f = (b and c) or (b and d) or (c and d)
				cons = (
					(perm[0] & perm[1]) |
					(perm[0] & perm[2]) |
					(perm[1] & perm[2])
				) ? cons : -cons;
			}
			//else
			else {
				//f = b xor c xor d
				cons = (perm[0] ^ perm[1] ^ perm[2]) ?
					cons : -cons;
			}

			res = fprintf(
				stream, "%d %d %d %d 0\n",
				-ante[0], -ante[1], -ante[2],
				cons
			);
			if (res < 0) {
				return -1;
			}
		}
	}
	
	return 0;
}

int fwriteTempClausesSHA1(
	FILE *		stream,
	const uint32_t	inp[],
	const uint32_t	oup[]
) {
	int res = 0,
	    ante[3] = { 0, 0, 0 },
	    cons[2] = { 0, 0 };

	uint32_t p = 0,
		 q = 0;

	bool tmp = 0,
	     crr = 0,
	     perm[3] = { 1, 1, 1 };

	for (int i = 0; i < (1 << 3); i++) {
		crr = 1;
		for (int j = 2; j >= 0 && crr > 0; j--) {
			tmp = perm[j] + crr;
			crr = tmp >> 1;
			perm[j] = tmp & 1;
		}

		for (int j = 0; j < 4; j++) {
			//outer add on the rhs
			if (i < (1 << 2)) {
				ante[0] = p * (perm[1] ? 1 : -1);
				ante[1] = q * (perm[2] ? 1 : -1);

				cons[0] = (perm[1] ^ perm[2]) ? 
					oup[j * 2] : -oup[j * 2];
				cons[1] = (perm[1] && perm[2]) ?
					oup[j * 2 + 1] : -oup[j * 2 + 1];

				res = fprintf(
					stream, "%d %d %d %d 0\n",
					-ante[0], -ante[1],
					cons[0], cons[1]
				);
				if (res < 0) {
					return -1;
				}
			}

			//inner add
			for (int k = 1; k < 32; k++) {
				ante[0] = (p + k) * (perm[0] ? 1 : -1);
				ante[1] = (q + k) * (perm[1] ? 1 : -1);
				ante[2] = oup[(j - 1) * 2 + 1] + k;

				cons[0] = (perm[0] ^ perm[1] ^ perm[2]) ?
					cons[0] : -cons[0];
				cons[1] = (
					(perm[0] | perm[1]) & perm[2]
				) ? cons[1] : -ante[1];

				res = fprintf(
					stream, "%d %d %d %d %d 0\n",
					-ante[0], -ante[1], -ante[2],
					cons[0], cons[1]
				);
				if (res < 0) {
					return -1;
				}
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
	int res = 0;
	uint32_t chcount = 0;

	uint32_t msa[80];
	uint32_t wvr[5] = { 0, 0, 0, 0, 0 };

	//for each chunk
	for (int i = 0; i < chcount; i++) {
		//word extension
		for (int j = 16; j < 80; j++) {
			if (res < 0) { 
				return res; 
			}
		}

		//compression function
		for (int j = 0; j < 80; j++) {
			if (j > 0 && j < 20) {
			}
			else if (j > 40 && j < 60) {
			}
			else {
			}
		}
	}

	return 0;	
}
