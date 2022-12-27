#include <NeoPixelBus.h>
#include "color.h"
#include "palette.h"
#include "anim.h"
#include "brightness.h"
#include "config.h"
#include "NeoPixelWrapper.cpp"
#include "math.h"

NeoPixelWrapper * strip;

Anim anim = Anim();

//TODO: hide inside the class, don't expose
int paletteInd = 0;
int animInd = 0;

//TODO: do something with this, it's bad thing to reference that var here
extern unsigned long ms;

Palette * pals[PALS] = {&PalRgb, &PalRainbow, &PalRainbowStripe, &PalParty, &PalHeat, &PalFire, &PalIceBlue, &PalXMas, &PalNY, &PalBLR, &PalUA};

Anim::Anim() 
{
    nextms = millis();
}

void Anim::setPeriod(byte period) {
    this->period = period;
}

void Anim::setPalette(int palind) {
    this->palette = pals[palind];
    if (setUpOnPalChange) {
        setUp();
    }
}

void Anim::run()
{    
    if ( millis()<=nextms) {
        return;
    }
  
    nextms=millis() + period;
    
    if (runImpl != NULL)
    {
        (this->*runImpl)();
    }

    //transition coef, if within 0..1 - transition is active
    //changes from 1 to 0 during transition, so we interpolate from current color to previous
    float transc = (float)((long)transms - (long)millis()) / TRANSITION_MS;
    Color * leds_prev = (leds == leds1) ? leds2 : leds1;
    int BRIGHTNESS = currentConfig.brightness;
    if (transc > 0) {
        for(int i=0; i<ledsNum; i++) {
            //transition is in progress
            Color c = leds[i].interpolate(leds_prev[i], transc);
            byte r = (int)pgm_read_byte_near(BRI + c.r) * BRIGHTNESS / 256;
            byte g = (int)pgm_read_byte_near(BRI + c.g) * BRIGHTNESS / 256;
            byte b = (int)pgm_read_byte_near(BRI + c.b) * BRIGHTNESS / 256;
            strip->SetPixelColor(i, RgbColor(r, g, b));
        }
    } else {
        for(int i=0; i<ledsNum; i++) {
            //regular operation
            byte r = (int)pgm_read_byte_near(BRI + leds[i].r) * BRIGHTNESS / 256;
            byte g = (int)pgm_read_byte_near(BRI + leds[i].g) * BRIGHTNESS / 256;
            byte b = (int)pgm_read_byte_near(BRI + leds[i].b) * BRIGHTNESS / 256;
            strip->SetPixelColor(i, RgbColor(r, g, b));
        }
    }
  
    strip->Show();
}

void Anim::setUp()
{
    if (!heightTransTable) {
        heightTransTable = (byte*)malloc(ledsNum);
      for (int i=0;i<ledsNum;i++) {
        heightTransTable[i] = (byte)floorf(255.0*(1.0-sqrtf(1.0-(float)i/(float)ledsNum)));
      }
    }

   //pinMode(LED_BUILTIN, OUTPUT);  
    transms = millis() + TRANSITION_MS;

    //switch operation buffers (for transition to operate)
    
    if (leds == leds1) {
        leds = leds2;
    } else {
        leds = leds1;
    }

    if (setUpImpl != NULL) {
        (this->*setUpImpl)();
    }
}

void Anim::doSetUp()
{
  if (!initialized) {
    currentConfig.configLoad();
    strip = new NeoPixelWrapper();
    
    ledsNum = currentConfig.leds;
    strip->Begin(currentConfig.neofeature, ledsNum, LED_PIN);
    initialized = true;
  }
  if (!setUpOnPalChange) {
      setUp();
  }
}

void Anim::setAnim(byte animInd)
{
    switch (animInd) {
        case 0:
            setUpImpl = &Anim::animStart_SetUp;
            runImpl = &Anim::animStart_Run;
            setUpOnPalChange = true;
        break;
        case 1: 
            setUpImpl = &Anim::animRun_SetUp;
            runImpl = &Anim::animRun_Run;
            setUpOnPalChange = true;
        break;
        case 2: 
            setUpImpl = &Anim::animPixieDust_SetUp;
            runImpl = &Anim::animPixieDust_Run;
            setUpOnPalChange = true;
        break;        
        case 3: 
            setUpImpl = &Anim::animSparkr_SetUp;
            runImpl = &Anim::animSparkr_Run;
            setUpOnPalChange = true;
        break;        
        case 4: 
            setUpImpl = &Anim::animRandCyc_SetUp;
            runImpl = &Anim::animRandCyc_Run;
            setUpOnPalChange = true;
        break;   
        case 5: 
            setUpImpl = &Anim::animStars_SetUp;
            runImpl = &Anim::animStars_Run;
            setUpOnPalChange = false;
        break;    
        case 6: 
            setUpImpl = &Anim::animSpread_SetUp;
            runImpl = &Anim::animSpread_Run;
            setUpOnPalChange = false;
        break;
        case 7: 
            setUpImpl = &Anim::animFly_SetUp;
            runImpl = &Anim::animFly_Run;
            setUpOnPalChange = false;
        break;                                
        case 8: 
            setUpImpl = &Anim::animPulse_SetUp;
            runImpl = &Anim::animPulse_Run;
            setUpOnPalChange = false;
        break;                                
        case 9: 
            setUpImpl = &Anim::animStatic_SetUp;
            runImpl = &Anim::animStatic_Run;
            setUpOnPalChange = false;
        break;
        case 100://special "magic" animation
            setUpImpl = &Anim::animMagic_SetUp;
            runImpl = &Anim::animMagic_Run;
            setUpOnPalChange = false;
        break;
        case ANIMATION_OTA_INDEX:// OTA
            setUpImpl = &Anim::animOTA_SetUp;
            runImpl = &Anim::animOTA_Run;
            setUpOnPalChange = false;
        break;
        default:
            setUpImpl = &Anim::animOff_SetUp;
            runImpl = &Anim::animOff_Run;
            setUpOnPalChange = false;
        break;
    }
}

//sets animation and palette to values specified in animInd and paletteInd, 
//and notifies all WebSocket connections of the change
void setAnimPal() {
  anim.setAnim(animInd);
  anim.setPeriod(random(20, 40));
  anim.setPalette(paletteInd);
  anim.doSetUp();
}

//sets animation and resets "change" time
void setAnimation(int animIndex)
{
    if ((animIndex < 0 || animIndex >= ANIMS) && (animIndex != 255) ) return; //TODO: get rid of magic number 255 ("OFF" animation)
    animInd = animIndex;
    setAnimPal();
    if (animInd == 0) {
        ms = millis() + 10000;  //startup animation requires less duration; TODO: generalize somehow these magic constants 10000 here and in main file
    } else {
        ms = millis() + INTERVAL;
    }
}

void setPalette(int paletteIndex)
{
    if (paletteInd < 0 || paletteInd >= PALS) return;
    paletteInd = paletteIndex;
    setAnimPal();
    ms = millis() + INTERVAL;
}

void setDuration(unsigned long durationMs) {
    ms = millis() + durationMs;
}

Color Anim::leds1[MAXLEDS];
Color Anim::leds2[MAXLEDS];
Color Anim::ledstmp[MAXLEDS];
int Anim::seq[MAXLEDS];
