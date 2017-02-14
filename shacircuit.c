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

int fwriteSigClausesSha(
	FILE * 		stream,
	size_t		wsize,
	index_t		sig,
	index_t		gen,
	index_t		x,
	uint32_t	r,
	uint32_t	s,
	uint32_t	t
) {
	int res = 0;
	bool comb[3] = { 0 },
	     eval = 0;

	atom_t ante[3] = { 0 },
	       cons = 0;

	res = fwriteRshClauses(
		stream,
		32,
		x,
		gen,
		t
	);
	if (res < 0) {
		return -1;
	}

	for (int i = 0; i < (1 << 3); i++) {
		*comb = nextCombination(comb, 3);
		eval = (comb[0] ^ comb[1] ^ comb[2]);

		for (int j = 0; j < wsize; j++) {
			ante[0] = signAtom(
				x + bitPosRro(32, j, r), comb[0]
			);
			ante[1] = signAtom(
				x + bitPosRro(32, j, s), comb[1]
			);
			ante[2] = signAtom(gen + j, comb[2]);

			cons = signAtom(sig + j, eval);

			res = fwriteClauses(
				stream, ante, 3, &cons, 1
			);
			if (res < 0) {
				return -1;
			}
		}
	}

	return gen + 32;
}

int fwriteEpClausesSha(
	FILE *		stream,
	size_t		wsize,
	index_t		ep,
	index_t		x,
	uint32_t	r,
	uint32_t	s,
	uint32_t	t
) {
	int res = 0;
	bool comb[3] = { 0 },
	     eval = 0;

	atom_t ante[3] = { 0 },
	       cons = 0;

	for (int i = 0; i < (1 << 3); i++) {
		*comb = nextCombination(comb, 3);
		eval = (comb[0] ^ comb[1] ^ comb[2]);

		for (int j = 0; j < wsize; j++) {
			ante[0] = signAtom(
				x + bitPosRro(32, j, r), comb[0]
			);
			ante[1] = signAtom(
				x + bitPosRro(32, j, s), comb[1]
			);
			ante[2] = signAtom(
				x + bitPosRro(32, j, t), comb[2]
			);

			cons = signAtom(ep + j, eval);

			res = fwriteClauses(
				stream, ante, 3, &cons, 1
			);
			if (res < 0) {
				return -1;
			}
		}
	}

	return 0;
}
int fwriteChClausesSha(
	FILE *		stream,
	size_t		wsize,
	index_t		ch,
	index_t		x,
	index_t		y,
	index_t		z
) {
	int res = 0;
	bool comb[3] = { 0 },
	     eval = 0;

	int ante[3] = { 0 },
	    cons =  0;

	for (int i = 0; i < (1 << 3); i++) {
		*comb = nextCombination(comb, 3);
		eval = (comb[0] & comb[1]) ^
		       (!comb[0] & comb[2]);

		for (int j = 0; j < wsize; j++) {
			ante[0] = signAtom(x + j, comb[0]);
			ante[1] = signAtom(y + j, comb[1]);
			ante[2] = signAtom(z + j, comb[2]);

			cons = signAtom(ch + j, eval);

			res = fwriteClauses(
				stream, ante, 3, &cons, 1
			);
			if (res < 0) {
				return -1;
			}
		}
	}
	
	return 0;
}

int fwriteParClausesSha(
	FILE *		stream,
	size_t		wsize,
	index_t		par,
	index_t		x,
	index_t		y,
	index_t		z
) {
	int res = 0;
	bool comb[3] = { 0 },
	     eval = 0;

	int ante[3] = { 0 },
	    cons = 0;

	for (int i = 0; i < (1 << 3); i++) {
		*comb = nextCombination(comb, 3);
		eval = comb[0] ^ comb[1] ^ comb[2];

		for (int j = 0; j < 32; j++) {
			ante[0] = signAtom(x + j, comb[0]);
			ante[1] = signAtom(y + j, comb[1]);
			ante[2] = signAtom(z + j, comb[2]);

			cons = signAtom(par + j, eval);
			
			res = fwriteClauses(
				stream, ante, 3, &cons, 1
			);
			if (res < 0) {
				return -1;
			}
		}
	}

	return 0;
}


