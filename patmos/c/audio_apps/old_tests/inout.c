#include <machine/spm.h>
#include <machine/rtc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "libaudio/audio.h"
#include "libaudio/audio.c"

/*
 * @file		Audio_InOut.c
 * @author	Daniel Sanz Ausin s142290 & Fabian Goerge s150957
 * @brief	This program takes the input auido data and outputs it again
 */


int main() {

    #if GUITAR == 1
    setup(1); //for guitar
    #else
    setup(0); //for volca
    #endif

    // enable input and output
    *audioDacEnReg = 1;
    *audioAdcEnReg = 1;

    setInputBufferSize(BUFFER_SIZE);
    setOutputBufferSize(BUFFER_SIZE);

    //AudioFX struct: contains no effect
    struct AudioFX audio1FX;
    struct AudioFX *audio1FXPnt = &audio1FX;
    int AUDIO_ALLOC_AMOUNT = alloc_dry_vars(audio1FXPnt, 0);

    while(*keyReg != 3) {
        audioIn(audio1FXPnt);
        audio_dry(audio1FXPnt);
        audioOut(audio1FXPnt);
    }

    return 0;
}
