// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <teak/teak.h>

int main(void)
{
    teakInit();

    uint16_t config;

    config = TMR_CONTROL_PRESCALE_16
           | TMR_CONTROL_MODE_ONCE
           | TMR_CONTROL_UNPAUSE | TMR_CONTROL_UNFREEZE_COUNTER
           | TMR_CONTROL_CLOCK_INTERNAL
           | TMR_CONTROL_AUTOCLEAR_OFF;

    timerStart(0, config, 0x8000000);

    config = TMR_CONTROL_PRESCALE_4
           | TMR_CONTROL_MODE_RELOAD
           | TMR_CONTROL_UNPAUSE | TMR_CONTROL_UNFREEZE_COUNTER
           | TMR_CONTROL_CLOCK_INTERNAL
           | TMR_CONTROL_AUTOCLEAR_OFF;

    timerStart(1, config, 0x8000000);

    while (1)
    {
        uint16_t data0 = apbpReceiveData(0);

        uint16_t rep0 = data0; // Heartbeat
        uint16_t rep1 = timerRead(0) >> 16;
        uint16_t rep2 = timerRead(1) >> 16;

        apbpSendData(0, rep0);
        apbpSendData(1, rep1);
        apbpSendData(2, rep2);
    }

    return 0;
}
