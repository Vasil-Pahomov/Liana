#include "anim.h"
#include "color.h"
#include "palette.h"
#include "math.h"

#define PULSE_LENGTH 100
#define PULSE_WIDTH 1
#define PULSE_DOT_WIDTH 1

const int PULSE_DOT[PULSE_DOT_WIDTH+2] = {
  0,
  255,
  0
};

const int PULSE_DATA[PULSE_LENGTH] = {
0,
50,
97,
140,
176,
204,
223,
234,
234,
226,
209,
183,
152,
114,
73,
31,
-13,
-54,
-93,
-126,
-154,
-176,
-189,
-195,
-193,
-183,
-167,
-144,
-116,
-84,
-50,
-15,
20,
53,
83,
109,
130,
144,
153,
155,
151,
142,
126,
107,
84,
58,
31,
3,
-23,
-48,
-69,
-87,
-101,
-111,
-115,
-115,
-110,
-101,
-88,
-73,
-55,
-36,
-16,
3,
21,
37,
51,
62,
70,
75,
76,
74,
69,
62,
52,
41,
30,
18,
6,
-5,
-15,
-23,
-29,
-34,
-36,
-37,
-36,
-33,
-29,
-24,
-19,
-14,
-9,
-4,
-1,
2,
4,
4,
3,
2
};

//seq[] keeps phases of pulses for corresponding led (which is the center of the pulse); value equal to PULSE_LENGTH means no pulse here
//ledstmp[] keeps color of corresponding pulse
void Anim::animPulse_SetUp() {
    Serial.print(F("(pulse)"));
    //inc is (average) interval between appearance of new pulses
    inc = 10 - ledsNum / 20;
    if (inc < 0) inc=0;

    //reset all phases
    for (int i=0; i<ledsNum; i++) {
      seq[i] = PULSE_LENGTH;
    }
    //seq[100] = 0;
    //ledstmp[100] = Color(255,255,255);
}

void Anim::animPulse_Run() {
    for (int i=0;i<ledsNum;i++) {
      leds[i].r = 0;
      leds[i].g = 0;
      leds[i].b = 0;
    }
    
    for (int p0=0;p0<ledsNum;p0++) {
      if (seq[p0] < PULSE_LENGTH) {
        int pd = 255+PULSE_DATA[seq[p0]];
        int p = p0 + floor(pd * PULSE_WIDTH / 256) - 1;
        int d = (pd * PULSE_WIDTH) % 256;
        //Serial.printf("%3d,%3d,%3d,%3d ... ", seq[p0], pd, p, d);
        for (int j=0;j<PULSE_DOT_WIDTH+1;j++) {
          int b = p + j - PULSE_DOT_WIDTH / 2;
          if (b>=0 && b < ledsNum) {
            Color col1 = ledstmp[p0].brightness(PULSE_DOT[j]);
            Color col2 = ledstmp[p0].brightness(PULSE_DOT[j+1]);
            Color colr = col1.interpolate(col2, (255-d)/255.0).brightness((PULSE_LENGTH - seq[p0]) * 255 / PULSE_LENGTH);
            leds[b].addSaturate(colr);
            //Serial.printf("%3d:%3d+%3d=%3d   ", b, col1.r, col2.r, leds[b].r);
          }
        }
        //Serial.printf("%4d%4d%4d%4d%4d%4d%4d\n",leds[97].r,leds[98].r,leds[99].r,leds[100].r,leds[101].r,leds[102].r,leds[103].r);
        seq[p0]++;
      }
    }

    if (random(inc) == 0) {
        int pos = PULSE_WIDTH + random(ledsNum-2*PULSE_WIDTH);
        if (seq[pos] == PULSE_LENGTH) {
            seq[pos] = 0;
            ledstmp[pos] = palette->getPalColor((float)rngb()/256);
        }
    }
}
