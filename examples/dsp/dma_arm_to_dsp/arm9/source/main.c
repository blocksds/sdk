// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

// This example shows how to transfer data from the ARM9 memory to the DSP. Note
// that DMA transfers are very delicate!
//
// It copies an array to the DSP, the DSP will add all the values in the array
// and return the sum. The ARM9 will then verify that it matches the expected
// result.
//
// In order for the DSP to see updated RAM values it is needed to flush the
// buffer in RAM from the ARM9.

#include <stdio.h>

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

    if (dspExecuteDefaultTLF(teak_tlf_bin) != DSP_EXEC_OK)
    {
        printf("Failed to execute TLF");

        while (1)
            swiWaitForVBlank();
    }

    // Fill buffer with test values
    for (int i = 0 ; i < BUFFER_SIZE; i++)
        buffer[i] = i + 1;
    for (int i = BUFFER_SIZE ; i < BUFFER_SIZE * 2; i++)
        buffer[i] = 0;

    DC_FlushRange(buffer, BUFFER_SIZE);

    uint16_t rep0 = 0;
    uint16_t rep1 = 0;
    uint16_t rep2 = 0;

    uint16_t cmd0 = 0;
    uint16_t cmd1 = 0;
    uint16_t cmd2 = 0;

    unsigned int transfer_size = 2;
    unsigned int dma_channel = 7;

    keysSetRepeat(20, 4);

    int offset = 0;

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

        consoleClear();

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

        unsigned int sum = (((uint32_t)rep1) << 16) | (uint32_t)rep2;

        unsigned int expected_sum = 0;
        for (int i = 0; i < transfer_size; i++)
            expected_sum += (unsigned int)buffer[i + offset / 2];

        printf("Sum:      %u\n", sum);
        printf("Expected: %u\n", expected_sum);
        printf("\n");
        printf("\n");

        printf("Press START to exit");
    }

    return 0;
}
