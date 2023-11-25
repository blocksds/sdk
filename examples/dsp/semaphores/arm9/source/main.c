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

    uint16_t rep0 = 0;
    uint16_t rep1 = 0;
    uint16_t rep2 = 0;

    uint16_t cmd0 = 0;

    int count = 0;

    int delay = 0;

    dspSetSemaphore(BIT(0));

    uint16_t last_semaphores = 0;

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysHeld();
        if (keys & KEY_START)
            break;

        printf("\x1b[2J"); // Clear console

        // CMD0/REP0 is used as a heartbeat counter

        cmd0 = count++;

        printf("CMD: %u\n\n\n", cmd0);

        if (dspSendDataReady(0))
            dspSendData(0, cmd0);

        // Once every 30 frames update the semaphores

        delay++;
        if (delay == 30)
        {
            delay = 0;

            uint16_t dsp_to_arm = dspGetSemaphore();
            if (dsp_to_arm != 0)
            {
                dspAckSemaphore(dsp_to_arm);

                uint16_t new_arm_to_dsp = dsp_to_arm << 1;
                if (new_arm_to_dsp == 0)
                    new_arm_to_dsp = BIT(0);
                dspSetSemaphore(new_arm_to_dsp);

                last_semaphores = new_arm_to_dsp;
            }
        }

        // Print debug information received from the DSP

        printf("DSP_PSTS: %04X %s\n\n\n", REG_DSP_PSTS,
               REG_DSP_PSTS & DSP_PSTS_SEM_FLAG ? "IRQ" : " ");

        if (dspReceiveDataReady(0))
            rep0 = dspReceiveData(0);
        if (dspReceiveDataReady(1))
            rep1 = dspReceiveData(1);
        if (dspReceiveDataReady(2))
            rep2 = dspReceiveData(2);

        printf("REP: %u %u %u\n\n\n", rep0, rep1, rep2);

        printf("DSP to ARM: %04X\n", last_semaphores);
        printf("ARM to DSP: %04X\n", rep1);
        printf("\n\n\n");

        printf("Press START to exit");
    }

    return 0;
}
