#include "circuit.h"

int fwriteOrLogic(
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

int fwriteXorLogic(
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
			clause[2] = (clause[0] < 0 != clause[1] < 0) ?
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

int fwriteNotXorLogic(
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
			clause[2] = (clause[0] > 0 != clause[1] < 0) ?
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

int fwriteRightShiftLogic(
	FILE *		stream,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	rsh, 
	uint32_t 	oup
) {
	int res = 0;
	int clause[2] = { 0, 0 };

	if (rsh > size) {
		rsh = size;
	}

	for (int i = size - rsh; i < size; i++) {
		res = fprintf(stream, "%d 0\n", -(oup + i));
		if (res < 0) {
			return res;
		}
	}

	for (int i = rsh; i < size; i++) {
		clause[0] = inp + i;
		clause[1] = oup + (i - rsh);

		res = fprintf( 
			stream, "%d %d 0\n %d %d 0\n",
			-clause[0], clause[1], clause[0], -clause[1]
		);
		if (res < 0) {
			return res;
		}
	}

	return 0;
}

int fwriteLeftShiftLogic(
	FILE *		stream,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	lsh, 
	uint32_t 	oup
) {
	int res = 0;
	int clause[2] = { 0, 0 };

	if (lsh > size) {
		lsh = size;
	}

	for (int i = 0; i < lsh; i++) {
		res = fprintf(stream, "%d 0\n", -(oup + i));
	}	

	for (int i = 0; i < size - lsh; i++) {
		clause[0] = inp + i;
		clause[1] = oup + (i + lsh);

		res = fprintf(
			stream, "%d %d 0\n %d %d 0\n",
			-clause[0], clause[1], clause[0], -clause[1]
		);
		if (res < 0) {
			return res;
		}
	}

	return 0;
}

int fwriteRightRotateLogic(
	FILE *		stream,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	rro, 
	uint32_t 	oup
) {
	int res = 0;
	int clause[2] = { 0, 0 };

	if (rro >= size) {
		rro = size % rro;
	}

	for (int i = 0; i < rro; i++) {
		clause[0] = inp + i;
		clause[1] = oup + (size - rro + i);

		res = fprintf(
			stream, "%d %d 0\n %d %d 0\n",
			-clause[0], clause[1], clause[0], -clause[1]
		);
		if (res < 0) {
			return res;
		}
	}

	for (int i = rro; i < size; i++) {
		clause[0] = inp + i;
		clause[1] = oup + (i - rro);

		res = fprintf( 
			stream, "%d %d 0\n %d %d 0\n",
			-clause[0], clause[1], clause[0], -clause[1]
		);
		if (res < 0) {
			return res;
		}
	}
	return 0;
}

int fwriteLeftRotateLogic(
	FILE *		stream,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	lro, 
	uint32_t 	oup
) {
	int res = 0;
	int clause[2] = { 0, 0 };

	if (lro >= size) {
		lro = size % lro;
	}

	for (int i = size - lro; i < size; i++) {
		clause[0] = inp + i;
		clause[1] = oup + (i - (size - lro));

		res = fprintf(
			stream, "%d %d 0\n %d %d 0\n",
			-clause[0], clause[1], clause[0], -clause[1]
		);
		if (res < 0) {
			return res;
		}
	}

	for (int i = 0; i < size - lro; i++) {
		clause[0] = inp + i;
		clause[1] = oup + (i + lro);

		res = fprintf(
			stream, "%d %d 0\n %d %d 0\n",
			-clause[0], clause[1], clause[0], -clause[1]
		);
		if (res < 0) {
			return res;
		}
	}

	return 0;
}

int fwriteSumLogic(
	FILE *		stream,
	size_t 		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t 	oup, 
	uint32_t 	crr
) {
	return 0;
}
