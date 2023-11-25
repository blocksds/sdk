// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

#include "teak_tlf_bin.h"

int main(int argc, char **argv)
{
    consoleDemoInit();

    if (!isDSiMode())
    {
        printf("DSP only available on DSi");
        while (1)
            swiWaitForVBlank();
    }

    if (dspExecuteDefaultTLF(teak_tlf_bin))
    {
        printf("Failed to execute TLF");
        while (1)
            swiWaitForVBlank();
    }

    int16_t rep0 = 0;
    int16_t rep1 = 0;
    int16_t rep2 = 0;

    int16_t cmd0 = 0;
    int16_t cmd1 = 0;
    int16_t cmd2 = 0;

    int count = 0;

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysHeld();
        if (keys & KEY_START)
            break;

        printf("\x1b[2J"); // Clear console

        cmd0 = count++;

        if (keys & KEY_TOUCH)
        {
            touchPosition touch;
            touchRead(&touch);

            cmd1 = touch.px;
            cmd2 = touch.py;
        }

        printf("Press the touch screen to\n"
               "change the values sent to\n"
               "the DSP.\n\n\n");

        printf("CMD: %u %u %u\n\n\n", cmd0, cmd1, cmd2);

        if (dspSendDataReady(0))
            dspSendData(0, cmd0);
        if (dspSendDataReady(1))
            dspSendData(1, cmd1);
        if (dspSendDataReady(2))
            dspSendData(2, cmd2);

        printf("DSP_PSTS: %04X\n\n\n", REG_DSP_PSTS);

        if (dspReceiveDataReady(0))
            rep0 = dspReceiveData(0);
        if (dspReceiveDataReady(1))
            rep1 = dspReceiveData(1);
        if (dspReceiveDataReady(2))
            rep2 = dspReceiveData(2);

        printf("REP: %u %d %d\n\n\n", rep0, rep1, rep2);

        printf("Press START to exit");
    }

    return 0;
}
