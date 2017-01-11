#include "sha1sat.h"

uint32_t indexHashBitSHA1(
	uint32_t 	chunk,
	uint32_t	word,
	uint32_t 	bit
) {
	return (chunk * SHA1_ATOMS_PER_CHUNK) + 
	       (word * 32) +
	       bit +
	       1;	//160 hash bits
}

uint32_t indexMessageScheduleBitSHA1(
	uint32_t	chunk,
	uint32_t	word,
	uint32_t	bit
) {
	return (chunk * SHA1_ATOMS_PER_CHUNK) +
	       (word * 32) +
	       bit +
	       161;	//2560 message schedule bits
}

uint32_t indexWorkingVarBitSHA1(
	uint32_t 	chunk,
	uint32_t 	word,
	uint32_t	bit
) {
	return (chunk * SHA1_ATOMS_PER_CHUNK) +
	       (word * 32) +
	       bit +
	       2721;	//15520 working variable bits
}

uint32_t indexBufferBitSHA1(
	uint32_t	chunk,
	uint32_t	word,
	uint32_t	bit
) {
	return (chunk * SHA1_ATOMS_PER_CHUNK) +
	       18241 +
	       (word * 32) +
	       bit;	//16896 buffer bits
}

int preprocessSHA1(
	FILE *		stream,
	size_t		msize
) {
	int res = 0;
	uint64_t osize = msize;

	//append the bit '1' to the message
	msize++;
	res = fwriteAtom(
		stream, indexMessageBitSHA1(osize, msize)
	);
	if (res < 0) {
		return res;
	}

	//append '0' to the message until message length % 512 is 448
	while (msize % 512 != 448) {
		msize++;
		res = fwriteAtom(
			stream, -indexMessageBitSHA1(osize, msize)
		);
		if (res < 0) {
			return res;
		}
	}

	//append original message length to the message as a 64 bit 
	//big-endian integer
	for (int i = 63; i >= 0; i--) {
		msize++;
		res = fwriteAtom(
			stream,
			((osize >> i & 1) == 1 ? 1 : -1) *
				indexMessageBitSHA1(osize, msize)
		);
		if (res < 0) {		
			return res;
		}
	}

	return msize;
}

int sha1sat(
	FILE * 		stream, 
	size_t 		msize, 
	const char * 	digest
) {
	int res = 0;

	//initialize working variables
	uint32_t a = 0,
		 b = 0,
		 c = 0,
		 d = 0,
		 e = 0,
		 f = 0;

	//initialize the default hash values
	uint32_t h0 = 0x67452301,
		 h1 = 0xEFCDAB89,
		 h2 = 0x98BADCFE,
		 h3 = 0x10325476,
		 h4 = 0xC3D2E1F0;
	
	//initialize the round constants
	uint32_t k[4] = { 
		0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6
	};

	//initialize message schedule array
	int w[80]; 	
	
	res = preprocessSHA1(stream, msize);
	if (res < 0) {
		return res;
	}

	const uint32_t chcount = res / 512;
	for (int i = 0; i < chcount; i++) {
		//break chunk into sixteen 32-bit words
		for (int j = 0; j < 16; j++) {	;
			w[j] = indexMessageScheduleBitSHA1(i, j, 0);

			res = fwriteAssignLogic(
				stream, 32, w[j],
				indexMessageScheduleBitSHA1(i, j, 0)
			);
			if (res < 0) {
				return res;
			}	
		}

		//word extension
		for (int j = 16; j < 80; j++) {
		}

		//compression function
		for (int j = 0; j < 80; j++) {
			
		}
	}
	

	return 0;	
}
