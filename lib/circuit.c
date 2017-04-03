#include "./circuit.h"

int fwrite_atom(FILE * stream, atom_t atom) {
	return fprintf(stream, "%d 0\n", atom);
}

int fwrite_word32_atoms(
	FILE *		stream,
	index_t		lhs,
	uint32_t 	word
) {
	int res = 0;

	for (int i = 0; i < 32; i++) {
		res = fwrite_atom(
			stream, sign_atom(lhs + i, word32_bit(word, i))
		);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

int fwrite_word64_atoms(
	FILE *		stream,
	index_t		lhs,
	uint64_t 	word
) {
	int res = 0;

	for (int i = 0; i < 64; i++) {
		res = fwrite_atom(
			stream, sign_atom(lhs + i, word64_bit(word, i))
		);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

int fwrite_clauses(
	FILE * 		    stream, 
	const atom_t *  ante, 
	size_t 		    asize, 
	const atom_t *  cons, 
	size_t 		    csize
) {
	int res = 0; 

	for (int i = 0; i < csize; i++) {
		for (int j = 0; j < asize; j++) {
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

int fwrite_iff_clauses(
	FILE *		stream,
	size_t		wsize,
	index_t		lhs,
	index_t		rhs
) {
	int res = 0;
	bool perm = 0;

	atom_t ante = 0,
	       cons = 0;

	for (int i = 0; i < 2; i++) {
		perm = !perm;

		for (int j = 0; j < 32; j++) {
			ante = sign_atom(rhs + j, perm);
			cons = sign_atom(lhs + j, perm);

			res = fwrite_clauses(
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
int fwrite_rsh_clauses(
	FILE *		stream,
	size_t		wsize,
	index_t		lhs,
	index_t		rhs,
	uint32_t	shift
) {
	int res = 0;

    for (int i = shift; i < wsize; i++) {
        res = fwrite_atom(stream, sign_atom(lhs + i, 0));
        if (res < 0) {
            return res;
        }

        res = fwrite_iff_clauses(stream, wsize, rhs + i, lhs + i - shift);
        if (res < 0) {
            return res;
        }
    }

    return 0;
}

//lhs = rhs << shift
int fwrite_lsh_clauses(
	FILE *		stream,
	size_t		wsize,
	index_t		lhs,
	index_t		rhs,
	uint32_t	shift
) {
	int res = 0;

	for (int i = 0; i < wsize - shift; i++) {
		res = fwrite_atom(stream, sign_atom(lhs + i, 0));
		if (res < 0) {
			return -1;
		}

		res = fwrite_iff_clauses(stream, wsize, rhs + i, lhs + i + shift);
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
	index_t		lhs,
	index_t		rhs,
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
	index_t		lhs,
	index_t		rhs,
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

//x1 + x2 + x3 ... + xn = lhs
int fwriteSumClauses(
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

	va_list args;
	va_start(args, count);

	index_t a = 0, 
		b = 0, 
		sum = va_arg(args, index_t), 
		crr = 0;

	//foreach combination of three unique atomic states
	for (int i = 0; i < (1 << 3); i++) {
		*comb = nextCombination(comb, 3);	
		eval[0] = comb[0] ^ comb[1] ^ comb[2];		//sum
		eval[1] = comb[0] + comb[1] + comb[2] > 1;	//carry

		//foreach '+' operator
		for (int j = 0; j < count; j++) {
			a = sum;
			b = va_arg(args, index_t);			

			//if this is the final '+' operator, use lhs
			sum = (j < count - 1) ? ++gen : lhs;
			crr = ++gen;

			//foreach bit
			for (int k = 0; k < wsize; k++) {	
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
	
	return gen;
}
