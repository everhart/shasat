#include "./circuit.h"

int fwriteAtoms32(
	FILE *		stream,
	const uint32_t	inp[],
	const uint32_t	oup[],
	uint64_t	leng
) {
	int res = 0,
	    atom = 0;

	for (int i = 0; i < leng; i++) {
		for (int j = 0; j < sizeof(uint32_t); j++) {
			atom = oup[i] * 
			       (inp[j] >> i & 1) ? 1 : -1;

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
	const uint32_t	inp[],
	const uint64_t	oup[],
	uint64_t	leng
) {
	int res = 0,
	    atom = 0;

	for (int i = 0; i < leng; i++) {
		for (int j = 0; j < sizeof(uint64_t); j++) {
			atom = oup[i] * 
			       (inp[j] >> i & 1) ? 1 : -1;

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

int fwriteAssignClauses(
	FILE *		stream,
	size_t		wsize,
	uint32_t	a,
	uint32_t	b
) {
	int res = 0;
	bool perm = 0;

	for (int i = 0; i < 2; i++) {
		perm = !perm;

		for (int j = 0; j < 32; j++) {
			int ante = signAtom(a + j, perm);
			int cons = signAtom(b + j, perm);

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
