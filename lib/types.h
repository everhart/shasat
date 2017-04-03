#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef int32_t atom_t;
typedef uint32_t index_t;

atom_t sign_atom(index_t atom, bool val);

#endif
