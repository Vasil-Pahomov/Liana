#include "effects.h"

typedef signed char         sint8_t;

void Glow::setUp()
{
    phaseSpd = random(8,13);
    freq = random(40,120);
}

Color Glow::apply(Color c, int i) const
{
    int bra = (sint8_t) (phase + i * freq);
    bra = BRA_OFFSET + (abs(bra) >> BRA_AMP_SHIFT);
    return c.brightness((int)bra);
}

void Glow::advance()
{
    phase += phaseSpd;
}
