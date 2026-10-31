#ifndef TIMEUTIL_H
#define TIMEUTIL_H

#include <Arduino.h>

// Returns true once millis() has reached (or passed) the deadline.
// Safe across the millis() wrap-around (every ~49.7 days), unlike a plain "millis() > deadline".
inline bool timeReached(unsigned long deadline) {
    return (long)(millis() - deadline) >= 0;
}

#endif
