#include "effects.h"

#define BOOM_PHASES 10 // number of phases in "boom" effect

// Spots controlled by the orientation of the user's device (see data/index.htm), with a "boom" gesture.
// setParams() is called from the WebSocket handler, run() renders the spots and fades everything else.

static Color magicGetColor(byte spotCol)
{
    Color col;

    if (spotCol <= 63) {
        col.r = spotCol*4;
        col.g = (63-spotCol)*4;
        col.b = 0;
    } else if (spotCol <= 127) {
        col.r = 255;
        col.g = (spotCol - 64)*4;
        col.b = col.g;
    } else if (spotCol <= 191) {
        col.r = (191 - spotCol) * 4;
        col.g = col.r;
        col.b = 255;
    } else {
        col.r = 0;
        col.g = (spotCol - 192) * 4;
        col.b = (255 - spotCol) * 4;
    }

    return col;
}

void EffectMagic::setUp() {
    boomWidth = ledsNum/BOOM_PHASES;
    for (int i=0;i<MAGIC_MAX_SPOTS;i++) {
        spots[i].boomPhase = 0;
        spots[i].color = 0;
        spots[i].boomPos = 0;
        spots[i].lb = spots[i].prevlb = ledsNum;
        spots[i].ub = spots[i].prevub = 0;
    }
}

void EffectMagic::run() {
    for (int i=0;i<ledsNum;i++) {
        leds[i].fade(5);
    }

    for (int i=0;i<MAGIC_MAX_SPOTS;i++) {
        Spot& spot = spots[i];
        Color col = magicGetColor(spot.color);

        if (spot.boomPhase > 0) {
            int boomdisp = (BOOM_PHASES - spot.boomPhase)*boomWidth;
            for (int j=0;j<boomWidth;j++) {
                int upos = spot.boomPos + boomdisp + j;
                int dpos = spot.boomPos - boomdisp - j;
                if (upos >=0 && upos < ledsNum) {
                    leds[upos] = col;
                }
                if (dpos >=0 && dpos < ledsNum) {
                    leds[dpos] = col;
                }
            }
            spot.boomPhase--;
        }

        if (spot.ub >= spot.lb) {
            int lb = spot.lb;
            int ub = spot.ub;

            if (spot.prevlb < lb) lb = spot.prevlb;
            if (spot.prevub > ub) ub = spot.prevub;

            for (int j=lb; j<=ub;j++) {
                leds[j] = col;
            }
        }

        spot.prevlb = spot.lb;
        spot.prevub = spot.ub;
        spot.lb = ledsNum;
        spot.ub = 0;
    }
}

void EffectMagic::setParams(byte ind, byte spotPosRel, byte spotCol, bool isBoom)
{
    if (ind >= MAGIC_MAX_SPOTS) return;
    int spotPos = spotPosRel*ledsNum >> 8;
    Spot& spot = spots[ind];

    if (spot.ub < spotPos) spot.ub = spotPos;
    if (spot.lb > spotPos) spot.lb = spotPos;

    spot.color = spotCol;

    if (isBoom) {
        spot.boomPhase = BOOM_PHASES;
        spot.boomPos = spotPos;
    }
    Serial.printf("MP: %d, PP:%d %s",spotPos, ind, isBoom ? "BOOM!" : "");Serial.println();
}

Effect* createEffectMagic() { return new EffectMagic(); }
