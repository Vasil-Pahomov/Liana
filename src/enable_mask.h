#ifndef ENABLE_MASK_H
#define ENABLE_MASK_H

#include <stdint.h>

// Bit masks of disabled items (effects or palettes): a set bit means "excluded from random cycling".
// Only the lowest `count` bits are meaningful. Header-only and Arduino-free so it can be unit tested on the host.

// number of enabled items among 0..count-1
inline int enabledCount(uint16_t disabledMask, int count) {
    int n = 0;
    for (int i = 0; i < count; i++) {
        if (((disabledMask >> i) & 1) == 0) n++;
    }
    return n;
}

// false for indices outside 0..count-1
inline bool isEnabled(uint16_t disabledMask, int index, int count) {
    if (index < 0 || index >= count) return false;
    return ((disabledMask >> index) & 1) == 0;
}

// returns the mask with the item enabled or disabled; indices outside 0..count-1 leave the mask unchanged
inline uint16_t withEnabled(uint16_t disabledMask, int index, int count, bool enabled) {
    if (index < 0 || index >= count) return disabledMask;
    uint16_t bit = (uint16_t)(1u << index);
    return enabled ? (uint16_t)(disabledMask & ~bit) : (uint16_t)(disabledMask | bit);
}

#endif
