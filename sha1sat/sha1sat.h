#ifndef SHA1SAT_H
#define SHA1SAT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/atom.h"
#include "../lib/circuit.h"
#include "../lib/bitops.h"
#include "../shacircuit.h"

int sha1sat(FILE * stream, size_t msize, const char * digest);

#endif
