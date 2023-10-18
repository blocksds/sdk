// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <inttypes.h>
#include <stdio.h>

#include <nds.h>

// This example allocates a buffer in main RAM, it writes some values there, it
// passes it to the ARM7, which adds all the values and returns the sum.

#define WAITING_VALUE 0x1234ABCD
volatile uint32_t response = WAITING_VALUE;

void FIFO_UserValue32Handler(u32 value, void *userdata)
{
    response = value;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    const uint32_t buffer_size = 123; // Words
    uint32_t *buffer = calloc(sizeof(uint32_t), buffer_size + 1);
    if (buffer == NULL)
    {
        printf("Couldn't allocate enough memory!\n");
    }
    else
    {
        // Setup the FIFO handler
        fifoSetValue32Handler(FIFO_USER_01, FIFO_UserValue32Handler, 0);

        // The first word of the buffer specifies the number of words contained
        // in the buffer.
        buffer[0] = buffer_size;
        for (uint32_t i = 0; i < buffer_size; i++)
            buffer[i + 1] = rand();

        // Send the buffer address to the ARM7, but flush the data cache before,
        // to ensure that the ARM7 sees the updated contents. If the ARM7
        // returned a buffer it would be needed to invalidate the data cache
        // over that buffer so that the ARM9 sees the updated contents.
        DC_FlushRange(buffer, buffer_size * sizeof(uint32_t));
        fifoSendAddress(FIFO_USER_01, buffer);

        uint32_t sum = 0;
        for (uint32_t i = 0; i < buffer_size; i++)
            sum += buffer[i + 1];

        printf("ARM9 sum: %" PRIu32 "\n", sum);

        while (response == WAITING_VALUE)
            swiWaitForVBlank();

        printf("ARM7 sum: %" PRIu32 "\n", response);

        printf("\n");

        free(buffer);
    }

    printf("Press START to exit");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysHeld();
        if (keys & KEY_START)
            break;
    }

    return 0;
}
