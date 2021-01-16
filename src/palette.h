#ifndef palette_h
#define palette_h

#include "color.h"


struct Palette
{
    int numColors;
    Color *colors;
    
    /**
    * Get the interpolated color from the palette.
    * The argument is a floating number between 0 and 1
    */
    Color getPalColor(float i) {
        if (i<1) {
            int i0 = (int)(i*(numColors-1));
            float t0 = i*(numColors-1) - truncf(i*(numColors-1));
            return colors[i0].interpolate(colors[i0+1], t0);
        }
        else {return colors[numColors-1];} //avoid t0==0 when i==1        
    }

    /**
     * Finds random color from the palette that is visually different from given color
     */
    Color getDifferentColor(Color c) {
        Color newColor = c;
        int cycle = 10;
        while (cycle-- > 0)
        {
            Color c2 = getPalColor((float)rngb()/256);
            if (c.colorDiff(c2) > c.colorDiff(newColor)) {
                newColor = c2;
            }
        }
        return newColor;
    }
       
};

////////////////////////////////////////////////////////////////////////////////
// Palette definitions
////////////////////////////////////////////////////////////////////////////////
extern Palette PalRgb;
extern Palette PalRainbow;
extern Palette PalRainbowStripe;
extern Palette PalParty;
extern Palette PalHeat;
extern Palette PalFire;
extern Palette PalIceBlue;
extern Palette PalXMas;
extern Palette PalBLR;

#endif
