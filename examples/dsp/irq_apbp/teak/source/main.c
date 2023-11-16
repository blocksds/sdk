// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <teak/teak.h>

uint16_t counter1 = 0;
uint16_t counter2 = 0;

// This function is defined as a weak symbol in the crt0, and it can be
// overwritten by the user.
void irqHandlerInt0(void)
{
    if (REG_APBP_STAT & APBP_STAT_CMD1_NEW)
    {
        counter1++;
        apbpReceiveData(1);
    }

    if (REG_APBP_STAT & APBP_STAT_CMD2_NEW)
    {
        counter2++;
        apbpReceiveData(2);
    }

    // Acknowledge interrupt before returning
    REG_ICU_IRQ_ACK = ICU_IRQ_MASK_APBP;
}

int main(void)
{
    teakInit();

    // Disable interrupts caused by the messages heartbeat channel, but leave
    // interrupts caused by channels 1 and 2 enabled.
    REG_APBP_CONTROL = APBP_CONTROL_IRQ_CMD0_DISABLE;

    // Enable and route APBP interrupt to CPU interrupt 0
    icuIrqSetup(ICU_IRQ_MASK_APBP, 0);

    // Enable CPU interrupts
    cpuEnableIrqs();
    cpuEnableInt0();

    while (1)
    {
        uint16_t rep0 = apbpReceiveData(0); // Heartbeat
        uint16_t rep1 = counter1;
        uint16_t rep2 = counter2;

        apbpSendData(0, rep0);
        apbpSendData(1, rep1);
        apbpSendData(2, rep2);
    }

    return 0;
}
