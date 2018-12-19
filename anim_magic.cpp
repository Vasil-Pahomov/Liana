#include <SoftwareSerial.h>
#include "anim.h"
#include "color.h"
#include "palette.h"

#define BOOM_PHASES 10//number of phases in "boom" effect
#define BOOM_WIDTH LEDS/BOOM_PHASES //width (in leds) of boom line
#define MAX_SPOTS 5 //maximum number of spots to track

typedef struct AMD {
  byte boomPhase, // "boom" phase
       color;     // spot color
  int lb, ub,     //current lower and upper bounds of the spot
      prevlb, prevub, //previous bounds
      boomPos;    //boom position
} AMD;

AMD amdata[MAX_SPOTS];

void Anim::animMagic_SetUp() {
  for (int i=0;i<MAX_SPOTS;i++) {
    amdata[i].lb = amdata[i].prevlb = LEDS;
    amdata[i].ub = amdata[i].prevub = 0;
  }
}

//pos denotes "spot" position (in leds)
//phase denotes color (0..255)

Color animMagic_getColor(byte spotCol)
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


void Anim::animMagic_Run() {

  for (int i=0;i<LEDS;i++) {
    leds[i].fade(5);
  }

  for (int i=0;i<MAX_SPOTS;i++) {
    
    Color col = animMagic_getColor(amdata[i].color);

    if (amdata[i].boomPhase > 0) {
      int boomdisp = (BOOM_PHASES - amdata[i].boomPhase)*BOOM_WIDTH;
      Serial.printf("BOOM: P=%d, pos=%d, disp=%d",amdata[i].boomPhase, amdata[i].boomPos, boomdisp);Serial.println();
      for (int j=0;j<BOOM_WIDTH;j++) {
        int upos = amdata[i].boomPos + boomdisp + j;
        int dpos = amdata[i].boomPos - boomdisp - j;
        if (upos >=0 && upos < LEDS) {
          leds[upos] = col;
        }
        if (dpos >=0 && dpos < LEDS) {
          leds[dpos] = col;
        }
      }
      amdata[i].boomPhase--;
    }
    
    if (amdata[i].ub >= amdata[i].lb) {
  
      int lb = amdata[i].lb;
      int ub = amdata[i].ub;
  
      if (amdata[i].prevlb < lb) lb = amdata[i].prevlb;
      if (amdata[i].prevub > ub) ub = amdata[i].prevub;
  
      //Serial.printf("CUR: %d-%d, EXT: %d-%d",animMagic_LB, animMagic_UB,lb,ub);Serial.println();
      
      for (int j=lb; j<=ub;j++)
      {
        leds[j] = col;
      }
    } 

    amdata[i].prevlb = amdata[i].lb;
    amdata[i].prevub = amdata[i].ub;
    amdata[i].lb = LEDS;
    amdata[i].ub = 0;

  }

}


void Anim::setMagicParams(byte ind, byte spotPos, byte spotCol, boolean isBoom) 
{
  if (runImpl == &Anim::animMagic_Run) {
    if (amdata[ind].ub < spotPos) amdata[ind].ub = spotPos;
    if (amdata[ind].lb > spotPos) amdata[ind].lb = spotPos;

    amdata[ind].color = spotCol;
    
    if (isBoom) {
      amdata[ind].boomPhase = BOOM_PHASES;
      amdata[ind].boomPos = spotPos;
    }
    Serial.printf("MP: %d, PP:%d %s",spotPos, ind, isBoom ? "BOOM!" : "");Serial.println();
  } 
}
