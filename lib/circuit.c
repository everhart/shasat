#include "circuit.h"

int fwriteOrLogic(
	FILE * 		stream,
       	size_t		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t	oup
) {
	int res = 0;
	int clause[3] = { 0, 0, 0 }; //clause buffer
	
	for (int i = 0; i < size; i++) {
		clause[0] = inp1 + i;
		clause[1] = inp2 + i;

		for (int j = 0; j < 4; j++) {
			clause[0] = -clause[0];
			clause[1] = (j % 2 == 0) ? 
				clause[1] : -clause[1];
			clause[2] = (clause[0] < 0 || clause[1] < 0) ?
				oup + i : -(oup + i);

			res = fprintf(
				stream, "%d %d %d 0\n", 
				clause[0], clause[1], clause[2]
			);
			if (res < 0) {
				return res;
			}
		}
	}

	return 0;
}

int fwriteNotOrLogic(
	FILE * 		stream,
	size_t		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t	oup	
) {
	int res = 0;
	int clause[3] = { 0, 0, 0 };

	for (int i = 0; i < size; i++) {
		clause[0] = inp1 + i;
		clause[1] = inp2 + i;

		for (int j = 0; j < inp2 + i; j++) {
			clause[0] = -clause[0];
			clause[1] = (j % 2 == 0) ?
				clause[1] : -clause[1];
			clause[2] = (clause[0] > 0 || clause[1] < 0) ?
				oup + i : (-oup + i);

			res = fprintf(
				stream, "%d %d %d 0\n", 
				clause[0], clause[1], clause[2]
			);
			if (res < 0) {
				return res;
			}	
		}
	}

	return 0;
}

int fwriteAndLogic(
	FILE *		stream,
	size_t 		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t 	oup
) {
	int res = 0;
	int clause[3] = { 0, 0, 0 };

	for (int i = 0; i < size; i++) {
		clause[0] = inp1 + i;
		clause[1] = inp2 + i;

		for (int j = 0; j < inp2 + i; j++) {
			clause[0] = -clause[0];
			clause[1] = (j % 2 == 0) ?
				clause[1] : -clause[1];
			clause[2] = (clause[0] < 0 && clause[1] < 0) ?
				oup + i : (-oup + i);

			res = fprintf(
				stream, "%d %d %d 0\n", 
				clause[0], clause[1], clause[2]
			);
			if (res < 0) {
				return res;
			}	
		}
	}

	return 0;
}

int fwriteNotAndLogic(
	FILE * 		stream,
	size_t 		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t 	oup
) {
	int res = 0;
	int clause[3] = { 0, 0, 0 };

	for (int i = 0; i < size; i++) {
		clause[0] = inp1 + i;
		clause[1] = inp2 + i;

		for (int j = 0; j < inp2 + i; j++) {
			clause[0] = -clause[0];
			clause[1] = (j % 2 == 0) ?
				clause[1] : -clause[1];
			clause[2] = (clause[0] > 0 && clause[1] < 0) ?
				oup + i : (-oup + i);

			res = fprintf(
				stream, "%d %d %d 0\n", 
				clause[0], clause[1], clause[2]
			);
			if (res < 0) {
				return res;
			}	
		}
	}

	return 0;
}

int fwriteXorLogic(
	FILE *		file,
	size_t 		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t 	res
) {
	return 0;
}

int fwriteRightShiftLogic(
	FILE *		file,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	rsh, 
	uint32_t 	res
) {
	return 0;
}

int fwriteLeftShiftLogic(
	FILE *		file,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	lsh, 
	uint32_t 	res
) {
	return 0;
}

int fwriteRightRotateLogic(
	FILE *		file,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	rro, 
	uint32_t 	res
) {
	return 0;
}

int fwriteLeftRotateLogic(
	FILE *		file,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	lro, 
	uint32_t 	res
) {
	return 0;
}

int fwriteSumLogic(
	FILE *		file,
	size_t 		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t 	res, 
	uint32_t 	crr
) {
	return 0;
}
