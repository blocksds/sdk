// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <teak/teak.h>

#define BUFFER_SIZE 512 // In words

ALIGN(0x1000) volatile uint16_t buffer[BUFFER_SIZE];

int main(void)
{
    teakInit();

    while (1)
    {
        uint16_t data0 = apbpReceiveData(0);
        uint16_t data1 = apbpReceiveData(1);
        uint16_t data2 = apbpReceiveData(2);

        uint16_t dma_channel = data0 >> 12;
        uint16_t size_words = data0 & 0xFFF;
        uint32_t base_address = (data1 << 16) | data2;

        uint32_t source_address = base_address;

        while (ahbmIsBusy());

        // Read data from the ARM9
        s16 ok = dmaTransferArm9ToDsp(dma_channel, source_address, &buffer[0],
                                      size_words);

        // Add all values
        uint32_t sum = 0;
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            uint32_t v = buffer[i];
            sum += v;
            buffer[i] = 0; // Clear buffer for the next copy
        }

        // Set this bit as an "ok" flag for the ARM9
        if (ok == 0)
            data0 |= 0x0200;

        apbpSendData(0, data0);
        apbpSendData(1, sum >> 16);
        apbpSendData(2, sum & 0xFFFF);
    }

    return 0;
}
