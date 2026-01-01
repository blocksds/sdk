// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024-2026

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    PrintConsole topScreen;
    PrintConsole bottomScreen1;
    PrintConsole bottomScreen2;

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);

    consoleInit(&bottomScreen1, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
    // We can copy the struct directly so that we use the same background layer,
    // tileset, map, etc, but change things like the dimensions of the window.
    bottomScreen2 = bottomScreen1;

    consoleSetWindow(&bottomScreen1, 2, 3,  // Top-left corner
                                     8, 8); // Size

    consoleSetWindow(&bottomScreen2, 15, 8,
                                     10, 5);

    consoleSelect(&bottomScreen1);

    printf("Printing in the bottom screen in a small window\n");

    consoleSelect(&bottomScreen2);

    printf("Printing in a different window now\n");

    consoleSelect(&topScreen);

    printf("Printing in the top screen\n");
    printf("\n");
    printf("\n");
    printf("Press START to exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
