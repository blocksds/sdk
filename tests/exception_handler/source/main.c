// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    defaultExceptionHandler();

    consoleDemoInit();

    // Clear console
    printf("\x1b[2J");
    printf("Press A to trigger an exception\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysHeld() & KEY_A)
            break;
    }

    // Write data to NULL to cause an exception
    *(uint32_t *)NULL = 0xDEAD;

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
