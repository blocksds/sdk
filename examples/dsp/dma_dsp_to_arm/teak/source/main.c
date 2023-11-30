// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <teak/teak.h>

#define BUFFER_SIZE 512 // In words

ALIGN(0x1000) volatile uint16_t buffer[BUFFER_SIZE];

int main(void)
{
    teakInit();

    uint16_t heartbeat = 0;
    int16_t ok = 0;

    while (1)
    {
        uint16_t data0 = apbpReceiveData(0);
        uint16_t data1 = apbpReceiveData(1);
        uint16_t data2 = apbpReceiveData(2);

        uint16_t dma_channel = data0 >> 12;
        uint16_t size_words = data0 & 0xFFF;
        uint32_t destination_address = ((uint32_t)data1 << 16) | data2;

        if (apbpGetSemaphore() & BIT(0))
        {
            apbpAckSemaphore(BIT(0));

            while (ahbmIsBusy());

            int i = 0;
            for ( ; i < size_words; i++)
                buffer[i] = i;
            for ( ; i < BUFFER_SIZE; i++)
                buffer[i] = 0;

            // Read data from the ARM9
            ok = dmaTransferDspToArm9(dma_channel, (const void *)&buffer[0],
                                      destination_address, size_words);

            apbpSetSemaphore(BIT(0));
        }

        // Set this bit as an "ok" flag for the ARM9
        if (ok == 0)
            data0 |= 0x0200;

        apbpSendData(0, data0);
        apbpSendData(1, size_words);
        apbpSendData(2, heartbeat++);
    }

    return 0;
}
