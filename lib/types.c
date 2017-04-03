#include "types.h"

atom_t sign_atom(index_t atom, bool val) {
	return (val) ? atom : -atom;
}
