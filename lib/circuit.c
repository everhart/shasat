#include "./circuit.h"

int fwriteAtom(FILE * stream, atom_t atom) {
	return fprintf(stream, "%d 0\n", atom) < 0 ? -1 : 0;
}

int fwriteAtoms32(
	FILE *		stream,
	const uint32_t	inp[],
	const index_t	oup[],
	size_t		leng
) {
	int res = 0,
	    atom = 0;

	for (int i = 0; i < leng; i++) {
		for (int j = 0; j < sizeof(uint32_t); j++) {
			atom = signAtom(oup[i], inp[j] >> i & 1);

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
	const uint64_t	inp[],
	const index_t	oup[],
	size_t		leng
) {
	int res = 0,
	    atom = 0;

	for (int i = 0; i < leng; i++) {
		for (int j = 0; j < sizeof(uint64_t); j++) {
			atom = signAtom(oup[j], inp[j] >> i & 1);

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
	index_t		inp,
	index_t		oup
) {
	int res = 0;
	bool perm = 0;

	for (int i = 0; i < 2; i++) {
		perm = !perm;

		for (int j = 0; j < 32; j++) {
			int ante = signAtom(inp + j, perm);
			int cons = signAtom(oup + j, perm);

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

	for (int i = 0; i < shift; i++) {
		res = fwriteAtom(
			stream, signAtom(lhs + i, 0)
		);
		if (res < 0) {
			return -1;
		}	
	}

	for (int i = shift; i < wsize; i++) {
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


