// SPDX-License-Identiier: CC0-1.0
//
// SPDX-FileContributor: Michele Di Giorgio, 2024
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <maxmod9.h>
#include <nds.h>

#include "soundbank.h"
#include "soundbank_bin.h"

#include <stdio.h>

const mm_word bitDepth = 16;
const mm_word samplingRate = 32768;  // Hertz
const mm_word reverbDelayLeft = 500; // milliseconds
const mm_word reverbDelayRight = 520; // milliseconds

int initReverb()
{
    // Enable reverb system
    mmReverbEnable();

    // Allocate reverb memory for right and left channels
    const size_t bufferLeftLen = mmReverbBufferSize(bitDepth,
                                                    samplingRate,
                                                    reverbDelayLeft);
    uint8_t *bufferLeft = (uint8_t *)malloc(bufferLeftLen * 4);
    if (bufferLeft == NULL)
    {
        printf("Not enough memory to allocate left channel buffer\n");
        return 1;
    }

    const size_t bufferRightLen = mmReverbBufferSize(bitDepth,
                                                     samplingRate,
                                                     reverbDelayRight);
    uint8_t *bufferRight = (uint8_t *)malloc(bufferRightLen * 4);
    if (bufferRight == NULL)
    {
        printf("Not enough memory to allocate right channel buffer\n");
        return 1;
    }

    // Configure reverb
    mm_reverb_cfg reverbCfg = { 0 };
    reverbCfg.flags = MMRF_MEMORY | MMRF_DELAY | MMRF_FEEDBACK | MMRF_PANNING |
                      MMRF_DRYLEFT | MMRF_DRYRIGHT | MMRF_RATE |
                      MMRF_16BITLEFT | MMRF_16BITRIGHT | MMRF_INVERSEPAN |
                      MMRF_BOTH;
    reverbCfg.memory = bufferLeft; // Set memory target (left channel)
    reverbCfg.delay = bufferLeftLen; // Set delay (left channel)
    reverbCfg.feedback = 1024; // Set feedback to 50% (both channels)
    reverbCfg.panning = 0; // Set panning to 0% (and inversed (100%) for right channel)
    reverbCfg.rate = 16777216 / samplingRate; // Set sampling rate for both channels
    mmReverbConfigure(&reverbCfg);

    // Configure the rest of the parameters
    reverbCfg.flags = MMRF_MEMORY | MMRF_DELAY | MMRF_RIGHT;
    reverbCfg.memory = bufferRight;     // Set memory target (right channel)
    reverbCfg.delay = bufferRightLen;   // Set delay (right channel)
    mmReverbConfigure(&reverbCfg);

    return 0;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    mmInitDefaultMem((mm_addr)soundbank_bin);

    mmLoad(MOD_JOINT_PEOPLE);

    printf("maxmod reverb example\n");
    printf("=====================\n");
    printf("\n");
    printf("A: Enable reverb\n");
    printf("B: Disable reverb\n");
    printf("\n");
    printf("START: Return to loader\n");

    // Init reverb
    if (initReverb() != 0) {
        printf("Error while initializing reverb\n");
        return 1;
    }

    soundEnable();

    mmStart(MOD_JOINT_PEOPLE, MM_PLAY_LOOP);

    bool reverbOn = false;

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys_down = keysDown();

        if (keys_down & KEY_START)
            break;

        if (keys_down & KEY_A)
        {
            if (!reverbOn)
            {
                mmReverbStart(MMRC_BOTH);
                reverbOn = true;
            }
        }

        if (keys_down & KEY_B)
        {
            if (reverbOn)
            {
                mmReverbStop(MMRC_BOTH);
                reverbOn = false;
            }
        }
    }

    mmStop();

    soundDisable();

    return 0;
}
