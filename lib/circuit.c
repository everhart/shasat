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
int fwrite_rro_clauses(
	FILE *		stream,
	size_t		wsize,
	index_t		lhs,
	index_t		rhs,
	uint32_t	rot
) {
	int res = 0;

	for (int i = 0; i < wsize; i++) {
		res = fwrite_iff_clauses(
			stream, wsize, rhs + i, lhs + bit_position_rro(wsize, i, rot)
		);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

//lhs = rhs lro rot
int fwrite_lro_clauses(
	FILE *		stream,
	size_t		wsize,
	index_t		lhs,
	index_t		rhs,
	uint32_t	rot
) {
	int res = 0;

	for (int i = 0; i < wsize; i++) {
		res = fwrite_iff_clauses(
			stream, wsize, rhs + i, lhs + bit_position_lro(wsize, i, rot)
		);
		if (res < 0) {
			return -1;
		}
	}

	return 0;
}

int fwrite_and_clauses(
    FILE *      stream,
    size_t      wsize,
    index_t     lhs,
    size_t      count,
    ...
) {
    int res = 0;
    atom_t * ante = (atom_t *)malloc(count * sizeof(atom_t)), cons = 0;
    va_list args;

    for (int i = 0; i < wsize; i++) {
        cons = (-lhs + i);
        
        va_start(args, count);
        for (int j = 0; j < count; j++) {
            ante[0] = (-va_arg(args, index_t) + i);

            res = fwrite_clauses(stream, ante, 1, &cons, 1);
            if (res < 0) {
                return res;
            }
        }
        va_end(args);

        va_start(args, count);
        for(int j = 0; j < count; j++) {
            ante[j] = va_arg(args, index_t);
        }
        va_end(args);

        cons = -cons;
        res = fwrite_clauses(stream, ante, count, &cons, 1);
        if (res < 0) {
            return res;
        }
    }

    return 0;
}

int fwrite_xor_clauses(
    FILE *      stream,
    size_t      wsize,
    index_t     lhs,
    size_t      count,
    ...
) {
    int res = 0;
    bool * comb = (bool *)malloc(count * sizeof(bool)), eval = 0;
    atom_t * ante = (atom_t *)malloc(count * sizeof(atom_t)), cons = 0;
    va_list args;

    for (int i = 0; i < (1 << count); i++) {
        *comb = next_combination(comb, count);

        eval = comb[0];
        for (int j = 1; j < count; j++) {
            eval ^= comb[j];
        }

        for (int j = 0; j < wsize; j++) {
            va_start(args, count);
            for (int k = 0; k < count; k++) {
                ante[k] = sign_atom(va_arg(args, index_t) + j, comb[k]);
            }
            va_end(args);

            cons = sign_atom(lhs + j, eval);
            res = fwrite_clauses(stream, ante, count, &cons, 1);
            if (res < 0) {
                return res;
            }
        }
    }

    return 0;
}

int fwrite_or_clauses(
    FILE *      stream,
    size_t      wsize,
    index_t     lhs,
    size_t      count,
    ...
) {
    int res = 0;
    atom_t * ante = (atom_t *)malloc(count * sizeof(atom_t)), cons = 0;
    va_list args;

    for (int i = 0; i < wsize; i++) {
        cons = lhs + i;

        va_start(args, count);
        for (int j = 0; j < count; j++) {
            ante[0] = va_arg(args, index_t) + i;

            res = fwrite_clauses(stream, ante, 1, &cons, 1);
            if (res < 0) {
                return res;
            }
        }
        va_end(args);

        va_start(args, count);
        for (int j = 0; j < count; j++) {
            ante[j] = -(va_arg(args, index_t) + i);
        }
        va_end(args);

        cons = -cons;
        res = fwrite_clauses(stream, ante, count, &cons, 1);
        if (res < 0) {
            return res;
        }
    }

    return 0;
}

static int fwrite_sum_clauses_non_va(
    FILE *      stream,
    size_t      wsize,
    index_t     lhs1,
    index_t     lhs2,
    index_t     rhs1,
    index_t     rhs2
) {
    int res = 0;
    bool comb[3] = { 0 }, eval[2] = { 0 };
    atom_t ante[3] = { 0 }, cons[2] = { 0 };

    for (int i = 0; i < (1 << 3); i++) {
        *comb = next_combination(comb, 3);
        eval[0] = comb[0] ^ comb[1] ^ comb[2];
        eval[1] = (comb[0] & comb[1]) | 
                  (comb[0] & comb[2]) |
                  (comb[1] & comb[2]);

        for (int j = 0; j < wsize; j++) {
            ante[0] = sign_atom(rhs1 + j, comb[0]);
            ante[1] = sign_atom(rhs2 + j, comb[1]);
            ante[2] = sign_atom(lhs2 + j - 1, comb[2]);
            cons[0] = sign_atom(lhs1 + j, eval[0]);
            cons[1] = sign_atom(lhs2 + j, eval[1]);

            if (i < (1 << 2) && j == 0) {
                res = fwrite_clauses(stream, ante, 2, cons, 2);
            }
            else {
                res = fwrite_clauses(stream, ante, 3, cons, 2);
            }

            if (res < 0) {
                return -1;
            }
        }
    }

    return 0;
}

//x1 + x2 + x3 ... + xn = lhs
atom_t fwrite_sum_clauses(
    FILE *      stream,
    size_t      wsize,
    index_t     lhs,
    index_t     gen,
    size_t      count,
    ...
) {
    int res = 0;
    index_t rhs1 = 0, rhs2 = 0, lhs1 = 0, lhs2 = 0;
    va_list args;
    
    va_start(args, count);
    lhs1 = va_arg(args, index_t);
    
    for (int i = 0; i < (count - 1); i++) {
        rhs1 = lhs1;
        rhs2 = va_arg(args, index_t);
        lhs1 = gen;
        lhs2 = gen + wsize;

        gen += wsize * 2;

        res = fwrite_sum_clauses(stream, wsize, rhs1, rhs2, lhs1, lhs2);
        if (res < 0) {
            return -1;
        }
    }

    rhs1 = lhs1;
    rhs2 = va_arg(args, index_t);
    
    res = fwrite_sum_clauses_non_va(stream, wsize, rhs1, rhs2, lhs, gen);
    if (res < 0) {
        return -1;
    }

    va_end(args);
    return gen;
}
