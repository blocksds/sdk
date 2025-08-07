// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    unsigned int level = PM_BACKLIGHT_MAX;
    unsigned int returned_level = systemSetBacklightLevel(level);

    while (1)
    {
        swiWaitForVBlank();

        consoleClear();
        printf("Requested level: %u\n", level);
        printf("\n");
        printf("UP/DOWN: Adjust requested level\n");
        printf("A:       Send command\n");
        printf("\n");
        printf("Returned level:  %u\n", returned_level);
        printf("\n");
        printf("Press START to exit");

        scanKeys();

        u16 keys = keysDown();

        if ((keys & KEY_UP) && (level < PM_BACKLIGHT_MAX))
            level++;
        if ((keys & KEY_DOWN) && (level > PM_BACKLIGHT_OFF))
            level--;

        if (keys & KEY_A)
            returned_level = systemSetBacklightLevel(level);

        if (keys & KEY_START)
            break;
    }

    return 0;
}
