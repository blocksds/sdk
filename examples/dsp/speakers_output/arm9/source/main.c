// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

// This example plays a song wigh Maxmod while the DSP generates a different
// square wave for each speaker (left/right). You can change the volume ratio
// between the ARM output and the DSP output (and fully silence one of the two,
// if you want). You can also change the frequency of the DSP waves to show that
// you can still communicate with the DSP while everything is happening.
//
// Only the ARM7 can change the ARM/DSP volume ratio, but libnds provides
// helpers to ask the ARM7 to adjust settings. Normally you would hardcode the
// ratio to a specific value after testing what works best for your application.

#include <stdio.h>

#include <maxmod9.h>
#include <nds.h>

#include "teak_tlf_bin.h"

#include "soundbank.h"
#include "soundbank_bin.h"

int main(int argc, char **argv)
{
    consoleDemoInit();

    if (!isDSiMode())
    {
        printf("DSP only available on DSi");
        while (1)
            swiWaitForVBlank();
    }

    if (!dspExecuteDefaultTLF(teak_tlf_bin))
    {
        printf("Failed to execute TLF");

        while (1)
            swiWaitForVBlank();
    }

    // Initialize maxmod and start playing a song
    mmInitDefaultMem((mm_addr)soundbank_bin);
    mmLoad(MOD_PARALLAX_80599);
    mmStart(MOD_PARALLAX_80599, MM_PLAY_LOOP);

    keysSetRepeat(20, 3);

    uint16_t rep0 = 0;
    uint16_t rep1 = 0;
    uint16_t rep2 = 0;

    uint16_t cmd0 = 0;
    uint16_t cmd1 = 0;
    uint16_t cmd2 = 0;

    uint16_t count = 0;
    uint16_t arm_dsp_ratio = 6;
    uint16_t frequency_left = 100;
    uint16_t frequency_right = 300;

    // Enable DSP sound output to the speakers
    soundExtEnable();
    soundExtSetRatio(arm_dsp_ratio);

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysDownRepeat();
        if (keys & KEY_START)
            break;

        if (keys & KEY_L)
        {
            if (arm_dsp_ratio < 8)
                arm_dsp_ratio++;
        }
        if (keys & KEY_R)
        {
            if (arm_dsp_ratio > 0)
                arm_dsp_ratio--;
        }

        // Set the requested ARM/DSP volume ratio
        soundExtSetRatio(arm_dsp_ratio);

        if (keys & KEY_UP)
        {
            if (frequency_left > 50)
                frequency_left--;
        }
        if (keys & KEY_DOWN)
        {
            if (frequency_left < 500)
                frequency_left++;
        }

        if (keys & KEY_X)
        {
            if (frequency_right > 50)
                frequency_right--;
        }
        if (keys & KEY_B)
        {
            if (frequency_right < 500)
                frequency_right++;
        }

        printf("\x1b[2J"); // Clear console

        printf("L/R: Sound ratio:\n");
        printf("\n");
        printf("    DSP: %d/8\n", 8 - arm_dsp_ratio);
        printf("    ARM: %d/8\n", arm_dsp_ratio);
        printf("\n");
        printf("\n");
        printf("DSP audio:\n");
        printf("\n");
        printf("Up/Down: Left freq: %u\n", frequency_left);
        printf("X/B: Right freq: %u\n", frequency_right);
        printf("\n");

        cmd0 = count++; // Heartbeat
        cmd1 = frequency_left;
        cmd2 = frequency_right;

        printf("CMD: 0x%04X 0x%04X 0x%04X\n", cmd0, cmd1, cmd2);

        if (dspSendDataReady(0))
            dspSendData(0, cmd0);
        if (dspSendDataReady(1))
            dspSendData(1, cmd1);
        if (dspSendDataReady(2))
            dspSendData(2, cmd2);

        if (dspReceiveDataReady(0))
            rep0 = dspReceiveData(0);
        if (dspReceiveDataReady(1))
            rep1 = dspReceiveData(1);
        if (dspReceiveDataReady(2))
            rep2 = dspReceiveData(2);

        printf("REP: 0x%04X 0x%04X 0x%04X\n", rep0, rep1, rep2);
        printf("\n");
        printf("\n");

        printf("Press START to exit");
    }

    return 0;
}
