#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int fwriteAtom(
	FILE *		stream,
	int		atom
);
int fwriteAssignLogic(
	FILE *		stream,
	size_t 		size,
	uint32_t 	inp,
	uint32_t 	oup
);
int fwriteOrLogic(
	FILE * 		stream,
       	size_t		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t	oup
);
int fwriteNotOrLogic(
	FILE *		stream,
	size_t		size,
	uint32_t	inp1,
	uint32_t	inp2,
	uint32_t	oup
);
int fwriteAndLogic(
	FILE *		stream,
	size_t 		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t 	oup
);
int fwriteNotAndLogic(
	FILE * 		stream,
	size_t 		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t 	oup
);
int fwriteXorLogic(
	FILE *		stream,
	size_t 		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t 	oup
);
int fwriteNotXorLogic(
	FILE *		stream,
	size_t 		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t 	oup
);
int fwriteRightShiftLogic(
	FILE *		stream,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	rsh, 
	uint32_t 	oup
);
int fwriteLeftShiftLogic(
	FILE *		stream,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	lsh, 
	uint32_t 	oup
);
int fwriteRightRotateLogic(
	FILE *		stream,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	rro, 
	uint32_t 	oup
);
int fwriteLeftRotateLogic(
	FILE *		stream,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	lro, 
	uint32_t 	oup
);
int fwriteSumLogic(
	FILE *		stream,
	size_t 		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t 	oup, 
	uint32_t 	crr
);

#endif
