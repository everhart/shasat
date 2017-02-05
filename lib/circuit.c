#include "./circuit.h"

int fwriteAtom(FILE * stream, atom_t atom) {
	return fprintf(stream, "%d 0\n", atom) < 0 ? -1 : 0;
}

int fwriteAtoms32(
	FILE *		stream,
	uint32_t	rhs,
	index_t 	lhs
) {
	int res = 0;

	for (int i = 0; i < sizeof(uint32_t); i++) {
		res = fwriteAtom(
			stream, 
			signAtom(lhs + i, bitVal32(rhs, i))
		);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

int fwriteAtoms64(
	FILE *		stream,
	uint64_t	rhs,
	index_t 	lhs
) {
	int res = 0;

	for (int i = 0; i < sizeof(uint64_t); i++) {
		res = fwriteAtom(
			stream, 
			signAtom(lhs + i, bitVal64(rhs, i))
		);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

int fwriteClauses(
	FILE * 		stream, 
	const atom_t	ante[], 
	size_t 		aleng, 
	const atom_t 	cons[], 
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

int fwriteAssignClauses(
	FILE *		stream,
	size_t		wsize,
	index_t		rhs,
	index_t		lhs
) {
	int res = 0;
	bool perm = 0;

	atom_t ante = 0,
	       cons = 0;

	for (int i = 0; i < 2; i++) {
		perm = !perm;

		for (int j = 0; j < 32; j++) {
			ante = signAtom(rhs + j, perm);
			cons = signAtom(lhs + j, perm);

			res = fwriteClauses(
				stream, &ante, 1, &cons, 1
			);
			if (res < 0) {
				return -1;
			}
		}
	}
	
	return 0;
}

//lhs = rhs >> shift
int fwriteRshClauses(
	FILE *		stream,
	size_t		wsize,
	index_t		rhs,
	index_t		lhs,
	uint32_t	shift
) {
	int res = 0;

	for (int i = shift; i < wsize; i++) {
		res = fwriteAtom(
			stream, signAtom(lhs + i, 0)
		);
		if (res < 0) {
			return -1;
		}

		res = fwriteAssignClauses(
			stream, wsize, rhs + i, lhs + i - shift
		);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

//lhs = rhs << shift
int fwriteLshClauses(
	FILE *		stream,
	size_t		wsize,
	index_t		rhs,
	index_t		lhs,
	uint32_t	shift
) {
	int res = 0;

	for (int i = 0; i < wsize - shift; i++) {
		res = fwriteAtom(
			stream, signAtom(lhs + i, 0)
		);
		if (res < 0) {
			return -1;
		}

		res = fwriteAssignClauses(
			stream, wsize, rhs + i, lhs + i + shift
		);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

//lhs = rhs rro rot
int fwriteRroClauses(
	FILE *		stream,
	size_t		wsize,
	index_t		rhs,
	index_t		lhs,
	uint32_t	rot
) {
	int res = 0;

	for (int i = 0; i < wsize; i++) {
		res = fwriteAssignClauses(
			stream, wsize, rhs + i, lhs + bitPosRro(wsize, i, rot)
		);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

//lhs = rhs lro rot
int fwriteLroClauses(
	FILE *		stream,
	size_t		wsize,
	index_t		rhs,
	index_t		lhs,
	uint32_t	rot
) {
	int res = 0;

	for (int i = 0; i < wsize; i++) {
		res = fwriteAssignClauses(
			stream, wsize, rhs + i, lhs + bitPosLro(wsize, i, rot)
		);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

//a + b = sum, crr
int fwriteSumClauses(
	FILE *		stream,
	size_t 		wsize,
	index_t		a,
	index_t		b,
	index_t		sum,
	index_t		crr
) {
	int res = 0;

	bool comb[3] = { 0 },
	     eval[2] = { 0 };

	atom_t ante[3] = { 0 },
	       cons[2] = { 0 };

	for (int i = 0; i < (1 << 3); i++) {
		*comb = nextCombination(comb, 3);

		eval[0] = comb[0] ^ comb[1] ^ comb[2];
		eval[1] = comb[0] + comb[1] + comb[2] > 1;

		//outer add
		if (i < (1 << 2)) {
			ante[0] = signAtom(a, comb[1]);
			ante[1] = signAtom(b, comb[2]);

			cons[0] = signAtom(sum, eval[0]),
			cons[1] = signAtom(crr, eval[1]);

			res = fwriteClauses(
				stream, ante, 2, cons, 2
			);
			if (res < 0) {
				return -1;
			}
		}

		//inner add
		for (int j = 1; j < wsize; j++) {
			ante[0] = signAtom(a + j, comb[0]),
			ante[1] = signAtom(b + j, comb[1]),
			ante[2] = signAtom(crr + j - 1, comb[2]);

			cons[0] = signAtom(sum + j, eval[0]),
			cons[1] = signAtom(crr + j, eval[1]);

			res = fwriteClauses(
				stream, ante, 3, cons, 3
			);
			if (res < 0) {
				return -1;
			}
		}
	}

	return 0;
}

int fwriteSumsClauses(
	FILE *		stream,
	size_t		wsize,
	uint32_t	lhs,
	atom_t		gen,
	uint32_t	count,
	...
) {
	int res = 0;
	
	bool comb[3] = { 0 },
	     eval[2] = { 0 };

	atom_t ante[3] = { 0 },
	       cons[2] = { 0 };

	index_t a = 0,
	       	b = 0, 
		sum = 0, 
		crr = 0;
	
	va_list args;
	va_start(args, count);

	//foreach combination of three unique atomic states
	for (int i = 0; i < (1 << 3); i++) {
		*comb = nextCombination(comb, 3);
		eval[0] = comb[0] ^ comb[1] ^ comb[2];
		eval[1] = comb[0] + comb[1] + comb[2] > 1;

		//foreach '+' operator
		for (int j = 0; j < count; j++) {
			a = sum;
			b = va_arg(args, index_t);			
			sum = ++gen;
			crr = ++gen;

			//foreach bit
			for (int k = 0; k < 32; k++) {
				ante[0] = signAtom(a + k, comb[1]);
				ante[1] = signAtom(a + k, comb[2]);
				cons[0] = signAtom(sum + k, eval[0]);
				cons[1] = signAtom(crr + k, eval[1]);

				//outer add
				if (k == 0 && i < (1 << 2)) {
					res = fwriteClauses(
						stream, ante, 2, cons, 2
					);
				}
				//inner add
				else {
					ante[2] = signAtom(
						crr + k - 1, comb[0]
					);

					res = fwriteClauses(
						stream, ante, 3, cons, 2
					);
					
				}
				
				if (res < 0) {
					return -1;
				}
			}
		}
	}
	
	return 0;
}
