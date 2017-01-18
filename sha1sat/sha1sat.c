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
	FILE *			stream,
	const uint32_t		atoms[]
) {
	int res = 0;

	bool tmp = 0, 
	     crr = 0,
	     perm[4][4];

	int ante[4] = { 0, 0, 0, 0 },
	    cons = 0;

	//generate all possible permutations of four atomic states
	//and store them in perm[][]
	for (int i = 0; i < (1 << 4); i++) {
		crr = 1;
		for (int j = 0; j >= 0 && crr > 0; j--) {
			tmp = perm[i][j] + crr;
			crr = tmp >> 1;
			perm[i][j] = tmp & 1;
		}
	}

	//for each bit
	for (int i = 0; i < 32; i++) {
		//for each permutation
		for (int j = 0; j < (1 << 4); j++) {
			//if permutation[j][idx] is true, then the atom 
			//in the antecedent is signed with NOT
			ante[0] = (atoms[0] + i) *
				  (perm[j][0] ? 1 : -1);
			ante[1] = (atoms[1] + i) * 
				  (perm[j][1] ? 1 : -1);
			ante[2] = (atoms[2] + i) * 
				  (perm[j][2] ? 1 : -1);
			ante[3] = (atoms[3] + i) * 
				  (perm[j][3] ? 1 : -1);

			//simple way to account for w[i] = w[i] lro 1
			cons = (i == 31) ? atoms[4] : atoms[4] + i; 

			//cons is signed with the result of the operation
			//w[i-3] xor w[i-8] xor w[i-14] xor w[i-16]
			cons = (
				perm[j][0] ^ 
				perm[j][1] ^ 
				perm[j][2] ^ 
				perm[j][3] 
			) ? cons : -cons;

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

	//for each permutation of three unique atom states
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

int fwriteWvrClausesSHA1(
	FILE *			stream,
	const uint32_t 		msa[],
	uint32_t		idx
) {
	int res = 0;
	int clause[5] = { 0, 0, 0, 0, 0};

	//for each bit	
	for (int i = 0; i < 32; i++) {
		clause[0] = msa[idx-3] + i;
		clause[1] = msa[idx-8] + i;
		clause[2] = msa[idx-14] + i;
		clause[3] = msa[idx-16] + i;

		//simple way of accounting for w[i] = w[i] lro 1
		clause[4] = (i == 31) ? msa[idx] : msa[idx] + i;

		//permutation loop for four unique variables
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
				clause[0], clause[1], 
				clause[2], clause[3],
				clause[4]
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
	const uint32_t	add[],
	uint32_t *	gen,
	uint32_t	idx
) {
	int res = 0;
	int clause[5] = { 0, 0, 0, 0 };

	for (int j = 0; j < 4; j++) {
		clause[0] = -clause[0];
		clause[1] = (j % 2 == 0) ? 
			clause[1] : -clause[1];
		
		clause[2] = (
			(clause[0] > 0) !=
			(clause[1] > 0) 
		) ? clause[3] : -clause[3];

		clause[3] = (
			(clause[0] > 0) &&
			(clause[1] > 0)
		) ? clause[4] : -clause[4];

		res = fprintf(
			stream, "%d %d %d 0\n %d %d %d 0\n",
			clause[0], clause[1], clause[2],
			clause[0], clause[1], clause[3]
		);
		if (res < 0) {
			return -1;
		}
	}

	for (int i = 0; i < 32; i++) {
		//outer add on the right side
		//permutation loop for two unique variables

		//inner add
		//permutation loop for three unique variables
		for (int j = 0; j < 8; j++) {
			clause[0] = -clause[0];
			clause[1] = (j % 2 == 0) ?
				clause[1] : -clause[1];
			clause[2] = (j % 3 == 0) ?	
				clause[2] : -clause[2];

			clause[3] = (
				(clause[0] > 0) !=
				(clause[1] > 0) !=
				(clause[2] > 0) 
			) ? clause[3] : -clause[3];

			clause[4] = (
				(clause[0] > 0) +
				(clause[1] > 0) +
				(clause[2] > 0) > 1
			) ? clause[4] : -clause[4];

			res = fprintf(
				stream, 
				"%d %d %d %d 0\n" 
				"%d %d %d %d 0\n",
				clause[0], clause[1], clause[2],
				clause[3],
				clause[0], clause[1], clause[2],
				clause[4]
			);
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
			res = fwriteWvrClausesSHA1(
				stream, msa, j
			);
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
