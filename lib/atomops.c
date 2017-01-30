#include "./atomops.h"

inline int signAtom(int atom, int state) {
	return (state) ? atom : -atom;
}
