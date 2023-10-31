// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <teak/teak.h>

#include "../../ipc.h"

int main(void)
{
    teakInit();

    uint16_t pattern[12] = {
        15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4
    };

    while (1)
    {
        uint16_t ret0 = (*(uint16_t *)(IPC_ARM9_TO_DSP_ADDR + 0) << 0)
                      | (*(uint16_t *)(IPC_ARM9_TO_DSP_ADDR + 1) << 4)
                      | (*(uint16_t *)(IPC_ARM9_TO_DSP_ADDR + 2) << 8)
                      | (*(uint16_t *)(IPC_ARM9_TO_DSP_ADDR + 3) << 12);

        uint16_t ret1 = (*(uint16_t *)(IPC_ARM9_TO_DSP_ADDR + 4) << 0)
                      | (*(uint16_t *)(IPC_ARM9_TO_DSP_ADDR + 5) << 4)
                      | (*(uint16_t *)(IPC_ARM9_TO_DSP_ADDR + 6) << 8)
                      | (*(uint16_t *)(IPC_ARM9_TO_DSP_ADDR + 7) << 12);

        uint16_t ret2 = (*(uint16_t *)(IPC_ARM9_TO_DSP_ADDR + 8) << 0)
                      | (*(uint16_t *)(IPC_ARM9_TO_DSP_ADDR + 9) << 4)
                      | (*(uint16_t *)(IPC_ARM9_TO_DSP_ADDR + 10) << 8)
                      | (*(uint16_t *)(IPC_ARM9_TO_DSP_ADDR + 11) << 12);

        for (int i = 0; i < 12; i++)
            *(uint16_t *)(IPC_DSP_TO_ARM9_ADDR + i) = pattern[i];

        // Send the values this way as well to verify the FIFO transfer
        apbpSendData(0, ret0);
        apbpSendData(1, ret1);
        apbpSendData(2, ret2);
    }

    return 0;
}
