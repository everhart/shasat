#ifndef SHA256SAT_H
#define SHA256SAT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/atom.h"
#include "../lib/bitops.h"
#include "../lib/circuit.h"
#include "../shacircuit.h"

int sha224sat(FILE * stream, size_t msize, const char * digest);
int sha256sat(FILE * stream, size_t msize, const char * digest);

#endif
