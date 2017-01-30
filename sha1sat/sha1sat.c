#include "sha1sat.h"

static uint32_t indexKBitSha1(
	uint32_t ccount, uint32_t idx, uint32_t bit
) {
	return 0;
}

static uint32_t indexMessageBitSha1(
	uint32_t ccount, uint32_t idx, uint32_t bit
) {
	return 0;
}

static uint32_t indexMsBitSha1(
	uint32_t chunk, uint32_t idx, uint32_t bit
) {
	return 0;
}

static uint32_t indexWvBitSha1(
	uint32_t kind, uint32_t chunk, uint32_t idx, uint32_t bit
) {
	return 0;
}

static uint32_t indexFBitSha1(
	uint32_t chunk, uint32_t idx, uint32_t bit
) {
	return 0;
}

static uint32_t indexTempBitSha1(
	uint32_t chunk, uint32_t idx, uint32_t bit
) {
	return 0;
}

static uint32_t indexHhBitSha1(
	uint32_t kind, uint32_t chunk, uint32_t bit
) {
	return 0;
}

int fwriteChunkClausesSHA1(
	FILE *		stream,
	uint32_t	inp,
	uint32_t	oup
) {
	int res = 0,
	    ante = 0,
	    cons = 0;

	for (int i = 0; i < 32; i++) {
		ante = inp + i;
		cons = oup + i;

		res = fprintf(
			stream, "%d %d 0\n %d %d 0\n",
			-ante, cons,
			ante, -cons
		);
		if (res < 0) {
			return res;
		}
	}

	return 0;
}

int fwriteMessageScheduleClausesSHA1(
	FILE *		stream,
	uint32_t	inp[4],
	uint32_t	oup
) {
	int res = 0;

	bool tmp = 0,
	     crr = 0,
	     perm[4] = { 0 };

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
			uint32_t ante[4] = {
				(inp[0] + j) * (perm[0] ? 1 : -1),
				(inp[1] + j) * (perm[1] ? 1 : -1),
				(inp[2] + j) * (perm[2] ? 1 : -1),
				(inp[3] + j) * (perm[3] ? 1 : -1),
			};

			uint32_t cons = (j < 30) ? oup : oup + j + 1;
			cons = (perm[0] ^ perm[1] ^ perm[2] ^ perm[3]) ?
				cons : -cons;

			//w[i] = 					
			//(w[i-3] xor w[i-8] xor w[i-14] xor w[i-16]) 
			//lro 1
			cons = (j == 31) ? oup : oup + j + 1;	//lro 1
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
	uint32_t		inp[3],
	uint32_t		oup,
	uint32_t		idx
) {
	int res = 0;
	bool tmp = 0, 
	     crr = 0,
	     perm[3] = { 0 };

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
			uint32_t ante[3] = {
				(inp[0] + j) * (perm[0] ? 1 : -1),
				(inp[1] + j) * (perm[1] ? 1 : -1),
				(inp[2] + j) * (perm[2] ? 1 : -1)
			};

			uint32_t cons = oup + j;

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

int fwriteTempClausesSHA1Re(
	FILE *		stream,
	uint32_t 	inp[5],
	uint32_t	oup[8]
) {
	int res = 0;

	uint32_t ante[3] = { 0 },
		 cons[2] = { 0 },
		 p = 0, 
		 q = 0;

	bool tmp = 0,
	     crr = 0,
	     perm[3] = { 0 };
	
	//for each permutation of three unique atom states
	for (int i = 0; i < (1 << 3); i++) {
		crr = 1;
		//modify perm[] to represent the next permutation
		for (int j = 2; j >= 0 && crr > 0; j--) {
			tmp = perm[j] + crr;
			crr = tmp >> 1;
			perm[j] = tmp & 1;
		}

		//for each '+' operator in the expression
		//temp = f + e + k + w[i] + (a lro 5)
		for (int j = 0; j < 5; j++) {
			//append clauses representing the outer add
			//operation
			if (i < (1 << 2)) {
				ante[0] = perm[1] ? p : -p;
				ante[1] = perm[2] ? q : -q;	

				cons[0] = oup[j * 2] *
					((perm[1] ^ perm[2]) ? 1 : -1);
				cons[1] = oup[j * 2 + 1] *  
					((perm[1] && perm[2]) ? 1 : -1);

				res = fprintf(
					stream, 
					"%1$d %2$d %3$d 0\n"
					"%1$d %2$d %4$d 0\n",
					-ante[0], -ante[1], 
					cons[0], cons[1]
				);
				if (res < 0) {
					return -1;
				}
			}

			//append clauses representing all inner add
			//operations
			for (int k = 1; k < 32; k++) {

			}
		}
	}

	return 0;
}

int fwriteWorkingVarClausesSHA1(
	FILE *		stream,
	const uint32_t	inp[],
	const uint32_t	oup[]
) {
	int res = 0;

	for (int j = 0; j < 5; j++) {
		for (int k = 0; k < 32; k++) { 
			uint32_t ante = inp[j],
				 cons = 0;

			if (j == 2) {
				cons = (k >= 2) ? 
					oup[j] + (k - 2) : oup[j] + k;
			} else {
				cons = oup[j] + k;
			}
			
			res = fprintf(
				stream, "%d %d 0\n %d %d 0\n", 
				-ante, cons,
				ante, -cons
			);
			if (res < 0) {
				return -1;
			}

		}
	}

	return 0;
}

int fwriteHashClausesSHA1(
	FILE *		stream,
	uint32_t	inp[10],
	uint32_t	oup[10]
) {
	int res = 0,
	    ante[3] = { 0 },
	    cons[2] = { 0 };

	uint32_t buf[4] = { 0 };

	bool tmp = 0,
	     crr = 0,
	     perm[3] = { 0 };

	for (int i = 0; i < (1 << 3); i++) {
		crr = 1;
		for (int j = 2; j >= 0 && crr > 0; j--) {
			tmp = perm[j] + crr;
			crr = tmp >> 1;
			perm[j] = tmp & 1;
		}

		for (int j = 0; j < 5; j++) {			
			buf[0] = inp[2 * j];
			buf[1] = inp[2 * j + 1];
			buf[2] = oup[2 * j];
			buf[3] = oup[2 * j + 1];

			if (i < (1 << 2)) {
				ante[0] = perm[1] ? buf[0] : -buf[0];
				ante[1] = perm[2] ? buf[1] : -buf[1];

				cons[0] = (perm[1] ^ perm[2]) ? 
					buf[2] : -buf[2];
				cons[1] = (perm[1] && perm[2]) ?
					buf[3] : -buf[3];


				res = fprintf(
					stream, 
					"%1$d %2$d %3$d 0\n"
					"%1$d %2$d %4$d 0\n",
					-ante[0], -ante[1], 
					cons[0], cons[1]	
				);
				if (res < 0) {
					return -1;
				}
			}

			for (int k = 1; k < 32; k++) {
				ante[0] = perm[0] ? 
					(buf[0] + k) : -(buf[0] + k);
				ante[1] = perm[1] ? 
					(buf[1] + k) : -(buf[1] + k);
				ante[2] = oup[(j - 1) * 2 + 1] + k;

				cons[0] = (perm[0] ^ perm[1] ^ perm[2]) ?
					oup[j * 2] : -oup[j * 2];
				cons[1] = (perm[0] | perm[1]) & perm[2] ?
					oup[j * 2 + 1] : -oup[j * 2 + 1];

				res = fprintf(
					stream, 
					"%1$d %2$d %3$d %4$d 0\n"
					"%1$d %2$d %3$d %5$d 0\n",
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

int sha1sat(
	FILE * 		stream, 
	size_t 		msize, 
	const char * 	digest
) {
	int res = 0;

	return 0;
}
