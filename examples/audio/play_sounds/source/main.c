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
    // Make sure that the sound engine on the ARM7 is initialized
    soundEnable();

    consoleDemoInit();

    // Variables to hold the channels being used by the sounds
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
            // Stop sound if it's already active
            if (ch_psg != -1)
                soundKill(ch_psg);

            ch_psg = soundPlayPSG(
                    DutyCycle_50,       // Duty cycle
                    440 + rand() % 256, // Random freq between 440 and 696 Hz
                    127,                // Volume: Max
                    64);                // Panning: Center
        }

        if (keys_down & KEY_Y)
        {
            // Stop sound if it's already active
            if (ch_wav != -1)
                soundKill(ch_wav);

            ch_wav = soundPlaySample(nature_raw_bin, // Pointer to sample
                        SoundFormat_16Bit,           // Format: Signed 16 bits
                        nature_raw_bin_size,         // Size in bytes of the sample
                        11025, // Frequency in Hz
                        127,   // Volume: Max
                        64,    // Panning: Center
                        false, // Disable looping
                        0);    // Loop start point (not used here)
        }

        if (keys_down & KEY_X)
        {
            // Stop sound if it's already active
            if (ch_noise != -1)
                soundKill(ch_noise);

            ch_noise = soundPlayNoise(
                    700 + rand() % 256, // Random freq between 700 and 956 Hz
                    127,                // Volume: Max
                    64);                // Panning: Center
        }

        if (keys_down & KEY_B)
        {
            // Stop any sound that is active

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
