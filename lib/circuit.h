#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <stdio.h>
#include <stdint.h>

int fwriteOrLogic(
	const char * 	file,
       	size_t		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t	res
);
int fwriteAndLogic(
	const char *	file,
	size_t 		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t 	res
);
int fwriteNotAndLogic(
	const char * 	file,
	size_t 		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t 	res
);
int fwriteXorLogic(
	const char *	file,
	size_t 		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t 	res
);
int fwriteRightShiftLogic(
	const char *	file,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	rsh, 
	uint32_t 	res
);
int fwriteLeftShiftLogic(
	const char *	file,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	lsh, 
	uint32_t 	res
);
int fwriteRightRotateLogic(
	const char *	file,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	rro, 
	uint32_t 	res
);
int fwriteLeftRotateLogic(
	const char *	file,
	size_t		size,
	uint32_t 	inp, 
	uint32_t 	lro, 
	uint32_t 	res
);
int fwriteSumLogic(
	const char *	file,
	size_t 		size,
	uint32_t 	inp1, 
	uint32_t 	inp2, 
	uint32_t 	res, 
	uint32_t 	crr
);

#endif
