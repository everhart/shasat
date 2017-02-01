#include "atom.h"

int signAtom(int atom, bool state) {
	return (state) ? atom : -atom;
}