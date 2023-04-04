// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

#define MASK_VALUE      0x0FFFFFFF
#define MASK_CHANNEL    0xF0000000

#define GET_VALUE(v)    ((v) & MASK_VALUE)
#define GET_CHANNEL(v)  (((v) & MASK_CHANNEL) >> 28)

#define SET_VALUE(v)    ((v) & MASK_VALUE)
#define SET_CHANNEL(v)  (((v) << 28) & MASK_CHANNEL)

unsigned int latest_command[8];
int error[8];

int thread_fifo(void *arg)
{
    uint32_t expected_channel = (uint32_t)arg;

    fifoSendValue32(expected_channel, SET_CHANNEL(expected_channel));

    while (1)
    {
        fifoWaitValueAsync32(expected_channel);

        uint32_t command = fifoGetValue32(expected_channel);

        int index = expected_channel - FIFO_USER_01;

        latest_command[index] = command;

        uint32_t value = GET_VALUE(command);
        uint32_t channel = GET_CHANNEL(command);

        // If the channel doesn't match, stop the loop
        if (channel != expected_channel)
        {
            error[index] = 1;
            return 0;
        }

        uint32_t response = SET_VALUE(value + 1) | SET_CHANNEL(channel);

        fifoSendValue32(expected_channel, response);
    }

    return 0;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    for (uint32_t ch = FIFO_USER_01; ch < FIFO_USER_08; ch++)
        cothread_create(thread_fifo, (void *)ch, 0, COTHREAD_DETACHED);

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);

        printf("\x1b[2J"); // Clear console

        for (uint32_t ch = FIFO_USER_01; ch < FIFO_USER_08; ch++)
        {
            int index = ch - FIFO_USER_01;

            printf("%X", latest_command[index]);

            if (error[index] != 0)
                printf(" [!]");

            printf("\n");
        }

        printf("\n\nPress START to exit");

        scanKeys();

        uint16_t keys = keysHeld();
        if (keys & KEY_START)
            break;
    }

    return 0;
}
