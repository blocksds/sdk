// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// This project shouldn't be able to build because the makefile expects a
// version of BlocksDS that is too high.

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("The project has been built!");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
