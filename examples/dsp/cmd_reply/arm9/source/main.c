// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

#include <nds/arm9/teak/dsp.h>

#include "dsp_tlf_bin.h"

static bool dsp_start(void)
{
    nwramSetBlockMapping(NWRAM_BLOCK_A, NWRAM_BASE, 0, NWRAM_BLOCK_IMAGE_SIZE_32K);

    // Map NWRAM to copy the DSP code
    nwramSetBlockMapping(NWRAM_BLOCK_B, 0x03000000, 256 * 1024,
                         NWRAM_BLOCK_IMAGE_SIZE_256K);
    nwramSetBlockMapping(NWRAM_BLOCK_C, 0x03400000, 256 * 1024,
                         NWRAM_BLOCK_IMAGE_SIZE_256K);

    if (!dspExecuteTLF(dsp_tlf_bin))
        return false;

    // Remove NWRAM from the memory map
    nwramSetBlockMapping(NWRAM_BLOCK_B, NWRAM_BASE, 0, NWRAM_BLOCK_IMAGE_SIZE_32K);
    nwramSetBlockMapping(NWRAM_BLOCK_C, NWRAM_BASE, 0, NWRAM_BLOCK_IMAGE_SIZE_32K);

    return true;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    dsp_start();

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
