// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    defaultExceptionHandler();

    consoleDemoInit();

    printf("Press A to trigger an exception\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysHeld() & KEY_A)
            break;
    }

    // Write data to an unmapped address to cause an exception
    // (The compiler can detect NULL being used here)
    *(uint32_t *)(0x00004000) = 0xDEAD;

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
