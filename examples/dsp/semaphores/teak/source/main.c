// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <teak/teak.h>

int main(void)
{
    teakInit();

    uint16_t last_semaphores = 0;

    while (1)
    {
        uint16_t data0 = apbpReceiveData(0);
        apbpSendData(0, data0);

        uint16_t arm_to_dsp = apbpGetSemaphore();
        if (arm_to_dsp != 0)
        {
            apbpAckSemaphore(arm_to_dsp);
            uint16_t new_arm_to_dsp = arm_to_dsp << 1;
            if (new_arm_to_dsp == 0)
                new_arm_to_dsp = BIT(0);
            apbpSetSemaphore(new_arm_to_dsp);

            last_semaphores = new_arm_to_dsp;
        }

        apbpSendData(1, last_semaphores);
        apbpSendData(2, 0);
    }

    return 0;
}
