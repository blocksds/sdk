// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>
#include <stdlib.h>

#include <nds.h>

void atexit_handler(void)
{
    printf("Test successful\n");
    swiDelay(10000000);
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("Atexit test\n");
    printf("--------------\n");
    printf("Press A to call exit(0)\n");
    printf("Press START to exit main()\n");

    atexit(atexit_handler);

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint32_t keys = keysHeld();
        if (keys & KEY_A)
            exit(0);
        if (keys & KEY_START)
            break;
    }

    return 0;
}
