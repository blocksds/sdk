// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdbool.h>
#include <stdio.h>

#include <nds.h>

// This file has been exported from Audacity as "Uncompressed", without header,
// with signed 16-bit values. Note that 8-bit audio must be exported as unsigned
// 8-bit values.
#include "nature_raw_bin.h"

int main(int argc, char **argv)
{
    soundEnable();

    consoleDemoInit();

    int ch_psg = -1;
    int ch_wav = -1;
    int ch_noise = 1;

    while (1)
    {
        swiWaitForVBlank();

        consoleClear();
        printf("A: PSG sound (channel: %d)\n", ch_psg);
        printf("Y: WAV sound (channel: %d)\n", ch_wav);
        printf("X: Noise sound (channel: %d)\n", ch_noise);
        printf("B: Stop sounds\n");
        printf("\n");
        printf("START: Return to loader\n");

        scanKeys();

        uint16_t keys_down = keysDown();

        if (keys_down & KEY_A)
        {
            if (ch_psg != -1)
                soundKill(ch_psg);

            ch_psg = soundPlayPSG(DutyCycle_50, 440 + rand() % 256, 127, 64);
        }

        if (keys_down & KEY_Y)
        {
            if (ch_wav != -1)
                soundKill(ch_wav);

            ch_wav = soundPlaySample(nature_raw_bin, SoundFormat_16Bit,
                                     nature_raw_bin_size, 11025, 127, 64,
                                     false, 0);
        }

        if (keys_down & KEY_X)
        {
            if (ch_noise != -1)
                soundKill(ch_noise);

            ch_noise = soundPlayNoise(700 + rand() % 256, 127, 64);
        }

        if (keys_down & KEY_B)
        {
            if (ch_psg != -1)
                soundKill(ch_psg);
            ch_psg = -1;

            if (ch_wav != -1)
                soundKill(ch_wav);
            ch_wav = -1;

            if (ch_noise != -1)
                soundKill(ch_noise);
            ch_noise = -1;
        }

        if (keys_down & KEY_START)
            break;
    }

    soundDisable();

    return 0;
}
