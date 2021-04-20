#include <machine/spm.h>
#include <machine/rtc.h>
#include <stdio.h>
#include <math.h>
#include "audio.h"
#include "audio.c"


int main() {

  short inL = 0;
  short inR = 0;

  //set buffer size
  setInputBufferSize(8);

  //enable ADC
  *audioAdcEnReg = 1;


  // audio data
  for(int i=0; i<4; i++) {
    getInputBuffer(&inL, &inR);
    printf("%x, %x\n", inL, inR);
  }
  printf("Done first.\nNow I am letting the buffer load a little bit to see what happens...\n");
  // audio data
  for(int i=0; i<10; i++) {
    getInputBuffer(&inL, &inR);
    printf("%x, %x\n", inL, inR);
  }

  //disable ADC
  *audioAdcEnReg = 0;

  printf("Done second.\n");

  return 0;
}
