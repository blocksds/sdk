// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <teak/teak.h>

uint16_t counter = 0;

// This function is defined as a weak symbol in the crt0, and it can be
// overwritten by the user.
void irqHandlerInt0(void)
{
    counter++;

    // Acknowledge interrupt before returning
    REG_ICU_IRQ_ACK = ICU_IRQ_MASK_TMR0;
}

int main(void)
{
    teakInit();

    // Setup timer 0 to restart from the reload value. Whenever it reaches 0,
    // timers cause a trap exception (not a timer interrupt!). The default trap
    // handler returns right after raising a timer 0 interrupt. This interrupt
    // can be routed to whichever CPU interrupt the user wants.

    uint16_t config = TMR_CONTROL_PRESCALE_16
           | TMR_CONTROL_MODE_RELOAD
           | TMR_CONTROL_UNPAUSE | TMR_CONTROL_UNFREEZE_COUNTER
           | TMR_CONTROL_CLOCK_INTERNAL
           | TMR_CONTROL_BREAKPOINT // Enable the trap signal when zero is reached
           | TMR_CONTROL_AUTOCLEAR_2_CYCLES; // Clear trap signal after 2 cycles

    timerStart(0, config, 0x800000);

    // Enable and route TMR0 interrupt to CPU interrupt 0
    icuIrqSetup(ICU_IRQ_MASK_TMR0, 0);

    // Enable CPU interrupts
    cpuEnableIrqs();
    cpuEnableInt0();

    while (1)
    {
        uint16_t rep0 = apbpReceiveData(0); // Heartbeat
        uint16_t rep1 = counter;
        uint16_t rep2 = timerRead(0) >> 16;

        apbpSendData(0, rep0);
        apbpSendData(1, rep1);
        apbpSendData(2, rep2);
    }

    return 0;
}
