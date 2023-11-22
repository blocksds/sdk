// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <teak/teak.h>

uint16_t left = 0;
uint16_t right = 0;
uint16_t left_count = 0;
uint16_t right_count = 0;
uint16_t frequency_left = 0;
uint16_t frequency_right = 0;

// This function is defined as a weak symbol in the crt0, and it can be
// overwritten by the user.
void irqHandlerInt1(void)
{
    if (left_count++ > frequency_left)
    {
        left_count = 0;
        left ^= 0x4000;
    }

    if (right_count++ > frequency_right)
    {
        right_count = 0;
        right ^= 0x4000;
    }

    while (btdmpTransmitFifoFull(0));
    REG_BTDMP_TRANSMIT_FIFO_DATA(0) = left;
    while (btdmpTransmitFifoFull(0));
    REG_BTDMP_TRANSMIT_FIFO_DATA(0) = right;

    REG_ICU_IRQ_ACK = ICU_IRQ_MASK_BTDMP0;
}

int main(void)
{
    teakInit();

    // Enable BTDMP channel 0 trigger interrupts in CPU interrupt 1
    btdmpSetupOutputSpeakers(0, 1);

    cpuEnableInt1();
    cpuEnableIrqs();

    while (1)
    {
        uint16_t data0 = apbpReceiveData(0);
        uint16_t data1 = apbpReceiveData(1);
        uint16_t data2 = apbpReceiveData(2);

        frequency_left = data1;
        frequency_right = data2;

        apbpSendData(0, data0);
        apbpSendData(1, left);
        apbpSendData(2, right);
    }

    return 0;
}
