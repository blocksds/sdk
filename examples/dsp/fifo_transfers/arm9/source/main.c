// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

// This example test the ARM9 functions that send data to the DSP data memory
// and receive data from it by using the FIFO registers.
//
// The ARM9 sends a bunch of zeroes to clear a buffer, and then it sends as many
// words as requested by the user (this way the DSP will see a buffer that
// starts with the pattern and ends with zeroes). This buffer is then compressed
// into 3 uint16_t values and returned to the ARM9 using the CMD/REP registers
// to verify that the transfer has worked.
//
// At the same time, the ARM9 requests to read words from DSP data memory, where
// the DSP has a specific pattern. The ARM9 reads as many words as requested by
// the user, and it compresses them into 3 uint16_t values as well.
//
// All the values are printed to the user so that it can be verified that the
// transfers work in both directions.

#include <stdio.h>

#include <nds.h>

#include "../../ipc.h"

#include "teak_tlf_bin.h"

const uint16_t zero = 0;

#define MAX_WRITTEN_WORDS 12
const uint16_t pattern[MAX_WRITTEN_WORDS] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
};

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

    // Expected values, received with the REP registers instead of the FIFO.
    uint16_t exp[3] = { 0 };

    uint16_t written_size = 8; // DSP words to write
    uint16_t read_size = 6; // DSP words to read

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysDown();
        if (keys & KEY_START)
            break;

        if (keys & KEY_UP)
        {
            if (written_size < MAX_WRITTEN_WORDS)
                written_size++;
        }
        if (keys & KEY_DOWN)
        {
            if (written_size > 1)
                written_size--;
        }

        if (keys & KEY_LEFT)
        {
            if (read_size > 1)
                read_size--;
        }
        if (keys & KEY_RIGHT)
        {
            if (read_size < MAX_WRITTEN_WORDS)
                read_size++;
        }

        printf("\x1b[2J"); // Clear console

        // Clear destination buffer in DSP memory
        dspFifoSend(DSP_PCFG_MEMSEL_DATA, &zero, true,
                    IPC_ARM9_TO_DSP_ADDR, false, MAX_WRITTEN_WORDS);
        // Copy as many words as requested to the DSP buffer
        dspFifoWriteData(&pattern[0], IPC_ARM9_TO_DSP_ADDR, written_size);

        // Clear buffer on the ARM9
        uint16_t read[MAX_WRITTEN_WORDS] = { 0 };
        // Read as many words as requested to the ARM9 buffer
        dspFifoReadData(IPC_DSP_TO_ARM9_ADDR, &read[0], read_size);

        // Compress all read words into 3 uint16_t values
        uint16_t print[3] = {
            (read[0] << 0) | (read[1] << 4) | (read[2] << 8) | (read[3] << 12),
            (read[4] << 0) | (read[5] << 4) | (read[6] << 8) | (read[7] << 12),
            (read[8] << 0) | (read[9] << 4) | (read[10] << 8) | (read[11] << 12)
        };

        // The DSP should have compressed the words we have sent into 3 uint16_t
        // values.
        if (dspReceiveDataReady(0))
            exp[0] = dspReceiveData(0);
        if (dspReceiveDataReady(1))
            exp[1] = dspReceiveData(1);
        if (dspReceiveDataReady(2))
            exp[2] = dspReceiveData(2);

        printf("REP:  0x%04X 0x%04X 0x%04X\n", exp[0], exp[1], exp[2]);
        printf("FIFO: 0x%04X 0x%04X 0x%04X\n", print[0], print[1], print[2]);
        printf("\n");
        printf("Sending %u words (Up/Down)\n", written_size);
        printf("Reading %u words (Left/Right)\n", read_size);
        printf("\n");
        printf("Press START to exit");
    }

    return 0;
}
