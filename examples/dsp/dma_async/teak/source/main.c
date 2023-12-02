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
    uint16_t delay_count = 0;

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

            delay_count = 0;

            // Read data from the ARM9
            ok = dmaTransferDspToArm9Async(dma_channel, (const void *)&buffer[0],
                                           destination_address, size_words);

            // Let's increment a counter while we wait for the DMA transfer to
            // be over, so that we can check that it's running at the same time
            // as we do other things. This will be passed to the ARM9 and
            // printed for the user to see it.
            while (dmaTransferIsRunning(dma_channel))
                delay_count++;

            apbpSetSemaphore(BIT(0));
        }

        // Set this bit as an "ok" flag for the ARM9
        if (ok == 0)
            data0 |= 0x0200;

        apbpSendData(0, data0);
        apbpSendData(1, delay_count);
        apbpSendData(2, heartbeat++);
    }

    return 0;
}
