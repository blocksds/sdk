// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

#define MASK_VALUE      0xFFFFFFF0
#define MASK_CHANNEL    0x0000000F

#define GET_VALUE(v)    (((v) & MASK_VALUE) >> 4)
#define GET_CHANNEL(v)  ((v) & MASK_CHANNEL)

#define SET_VALUE(v)    (((v) << 4) & MASK_VALUE)
#define SET_CHANNEL(v)  ((v) & MASK_CHANNEL)

unsigned int latest_command[8][3];
int error[8];

int thread_fifo_value32(void *arg)
{
    uint32_t expected_channel = (uint32_t)arg;

    uint32_t start_value = SET_VALUE((expected_channel - FIFO_USER_01) << 24) |
                           SET_CHANNEL(expected_channel);

    fifoSendValue32(expected_channel, start_value);

    while (1)
    {
        fifoWaitValue32Async(expected_channel);

        uint32_t command = fifoGetValue32(expected_channel);

        int index = expected_channel - FIFO_USER_01;

        latest_command[index][0] = command;

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

int thread_fifo_address(void *arg)
{
    uint32_t expected_channel = (uint32_t)arg;

    uint32_t start_value = SET_VALUE(0x200000) | SET_CHANNEL(expected_channel);
    fifoSendAddress(expected_channel, (void *)start_value);

    while (1)
    {
        fifoWaitAddressAsync(expected_channel);

        uint32_t command = (uint32_t)fifoGetAddress(expected_channel);

        int index = expected_channel - FIFO_USER_01;

        latest_command[index][1] = command;

        uint32_t value = GET_VALUE(command);
        uint32_t channel = GET_CHANNEL(command);

        // If the channel doesn't match, stop the loop
        if (channel != expected_channel)
        {
            error[index] = 1;
            return 0;
        }

        uint32_t response = SET_VALUE(value + 1) | SET_CHANNEL(channel);

        fifoSendAddress(expected_channel, (void *)response);
    }

    return 0;
}

typedef struct {
    uint8_t channel;
    uint32_t value;
} datamsg;

int thread_fifo_datamsg(void *arg)
{
    uint32_t expected_channel = (uint32_t)arg;

    datamsg msg_send = {expected_channel, 0};

    fifoSendDatamsg(expected_channel, sizeof(msg_send), (void *)&msg_send);

    while (1)
    {
        fifoWaitDatamsgAsync(expected_channel);

        datamsg msg_recv;

        fifoGetDatamsg(expected_channel, sizeof(msg_recv), (void *)&msg_recv);

        int index = expected_channel - FIFO_USER_01;

        latest_command[index][2] = msg_recv.value | ((uint32_t)msg_recv.channel << 28);

        // If the channel doesn't match, stop the loop
        if (msg_recv.channel != expected_channel)
        {
            error[index] = 1;
            return 0;
        }

        msg_send.value = msg_recv.value;
        fifoSendDatamsg(expected_channel, sizeof(msg_send), (void *)&msg_send);
    }

    return 0;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    for (uint32_t ch = FIFO_USER_01; ch <= FIFO_USER_08; ch++)
    {
        cothread_create(thread_fifo_value32, (void *)ch, 0, COTHREAD_DETACHED);
        cothread_create(thread_fifo_address, (void *)ch, 0, COTHREAD_DETACHED);
        cothread_create(thread_fifo_datamsg, (void *)ch, 0, COTHREAD_DETACHED);
    }

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);

        consoleClear();

        for (uint32_t ch = FIFO_USER_01; ch <= FIFO_USER_08; ch++)
        {
            int index = ch - FIFO_USER_01;

            for (int i = 0; i < 3; i++)
                printf("%08X ", latest_command[index][i]);

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
