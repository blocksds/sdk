// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

// This example lets you record the microphone input and play it back with the
// DSP directly, without ever using the ARM7. You can record about two seconds
// of audio from the moment you press the button.

#include <stdio.h>

#include <nds.h>

#include "teak_tlf_bin.h"

#define CMD_NONE    0
#define CMD_RECORD  1
#define CMD_PLAY    2

int main(int argc, char **argv)
{
    consoleDemoInit();

    if (!isDSiMode())
    {
        printf("DSP only available on DSi");
        while (1)
            swiWaitForVBlank();
    }

    if (dspExecuteDefaultTLF(teak_tlf_bin) != DSP_EXEC_OK)
    {
        printf("Failed to execute TLF");

        while (1)
            swiWaitForVBlank();
    }

    uint16_t rep0 = 0;
    uint16_t rep1 = 0;
    uint16_t rep2 = 0;

    uint16_t cmd0 = 0;
    uint16_t cmd1 = 0;
    uint16_t cmd2 = 0;

    uint16_t count = 0;

    uint16_t command = 0;
    uint16_t command_count = 0;

    // Enable DSP sound output to the speakers
    soundExtSetRatio(0); // 100% DSP output

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysDown();
        if (keys & KEY_START)
            break;

        if (keys & KEY_X)
        {
            command = CMD_RECORD;
            command_count++;

            // It is required to power on the microphone to record audio!
            soundMicPowerOn();
        }
        if (keys & KEY_A)
        {
            command = CMD_PLAY;
            command_count++;

            // Power the microphone off when it isn't used.
            soundMicPowerOff();
        }

        printf("\x1b[2J"); // Clear console
        printf("X: Record\n");
        printf("A: Play\n");
        printf("\n");

        cmd0 = count++; // Heartbeat
        cmd1 = command;
        cmd2 = command_count;

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
