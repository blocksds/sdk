// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: ds-sloth, 2024

#include "cross_platform_logic.h"

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    int result = cross_platform_program();

    printf("The answer is... %d\n", result);

    printf("Press START to exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
