// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

// This example shows how to transfer data from the DSP memory to the ARM9. Note
// that DMA transfers are very delicate!
//
// It copies an array from the DSP. The DSP will add all the values and the ARM9
// will add all the values and print the sum.
//
// In order to synchronize the transfer, semaphores are used.
//
// In order for the ARM9 to see updated RAM values it is needed to flush the
// buffer in RAM.

#include <stdio.h>
#include <string.h>

#include <nds.h>

#include "teak_tlf_bin.h"

#define BUFFER_SIZE 512 // In DSP words

// DMA copies can't cross 1 KB boundaries because of limitations of the DSP AHBM
// bus, so this has to be aligned to 1 KB. However, copies can be buggy unless
// this is aligned to a bigger boundary, so it is actually aligned to 4 KB.
//
// The first half is full of increasing numbers, the second half is filled with
// zeroes
ALIGN(4096) uint16_t buffer[BUFFER_SIZE * 2];

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

    for (int i = 0; i < BUFFER_SIZE; i++)
        buffer[i] = 0;

    uint16_t rep0 = 0;
    uint16_t rep1 = 0;
    uint16_t rep2 = 0;

    uint16_t cmd0 = 0;
    uint16_t cmd1 = 0;
    uint16_t cmd2 = 0;

    unsigned int transfer_size = 2;
    unsigned int dma_channel = 7;

    int offset = 0;

    keysSetRepeat(20, 4);

    unsigned int sum = 0;

    // Tell the DSP to start the first transfer
    dspSetSemaphore(BIT(0));

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysDownRepeat();
        if (keys & KEY_START)
            break;

        if (keys & KEY_UP)
            transfer_size++;
        if (keys & KEY_DOWN)
            transfer_size--;
        transfer_size &= 255;

        if (keys & KEY_RIGHT)
            dma_channel++;
        if (keys & KEY_LEFT)
            dma_channel--;
        dma_channel &= 7;

        if (keys & KEY_R)
            offset += 2;
        if (keys & KEY_L)
            offset -= 2;
        offset &= 255;

        printf("\x1b[2J"); // Clear console

        printf("Up/Down:    Size %u words\n", transfer_size);
        printf("Left/Right: DMA ch %u (!= 0)\n", dma_channel);
        printf("L/R:        Offset %u bytes\n", offset);
        printf("\n");

        uintptr_t address = offset + (uintptr_t)&buffer[0];

        cmd0 = (transfer_size & 0xFFF) | (dma_channel << 12);
        cmd1 = address >> 16;
        cmd2 = address & 0xFFFF;

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

        if (dspGetSemaphore() & BIT(0))
        {
            dspAckSemaphore(BIT(0));

            DC_FlushRange(buffer, BUFFER_SIZE);

            sum = 0;
            for (int i = 0; i < BUFFER_SIZE; i++)
                sum += (unsigned int)buffer[i];

            dspSetSemaphore(BIT(0));

            memset(buffer, 0, BUFFER_SIZE);

            DC_FlushRange(buffer, BUFFER_SIZE);
        }

        printf("Sum: %u\n", sum);
        printf("\n");
        printf("\n");

        printf("Press START to exit");
    }

    return 0;
}
