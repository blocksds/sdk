// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2024

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    defaultExceptionHandler();

    consoleDemoInit();

    printf("A: Trigger exception on ARM9\n");
    printf("B: Trigger exception on ARM7\n\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        uint16_t keys = keysHeld();

        if (keys & KEY_A)
        {
            printf("Causing exception on ARM9\n\n");

            // Write data to an unmapped address to cause an exception
            // (The compiler can detect NULL being used here)
            *(uint32_t *)(0x00004000) = 0xDEAD;
            break;
        }

        if (keys & KEY_B)
        {
            printf("Causing exception on ARM7\n\n");

            fifoSendValue32(FIFO_USER_07, 0);
            break;
        }
    }

    printf("Exception handling failed!\n");
    printf("Is this in an emulator?\n");
    printf("\n");
    printf("Press START to exit\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
