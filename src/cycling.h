#ifndef CYCLING_H
#define CYCLING_H

#include <stdint.h>
#include "enable_mask.h"

// Random choice for automatic cycling. Header-only and Arduino-free so it can be unit tested on the host.

// returns a value in [from, to)
typedef int (*RandomFn)(int from, int to);

// Picks a random index in [from, to) that is enabled in the mask and differs from `exclude`
// (pass -1 to exclude nothing). Every candidate is equally likely. Returns -1 when there is no candidate.
inline int pickRandomEnabled(uint16_t disabledMask, int count, int from, int to, int exclude, RandomFn rnd) {
    int candidates = 0;
    for (int i = from; i < to; i++) {
        if (i != exclude && isEnabled(disabledMask, i, count)) candidates++;
    }
    if (candidates == 0) return -1;

    int n = rnd(0, candidates);
    for (int i = from; i < to; i++) {
        if (i != exclude && isEnabled(disabledMask, i, count)) {
            if (n == 0) return i;
            n--;
        }
    }
    return -1;
}

#endif
