#include "./atomops.h"

int signAtom(int atom, int state) {
	return (state) ? atom : -atom;
}
