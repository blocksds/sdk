// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    PrintConsole topScreen;
    PrintConsole bottomScreen;

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    int x = 2;
    int y = 3;
    int width = 6;
    int height = 5;
    consoleSetWindow(&bottomScreen, x, y, x + width, y + height);

    consoleSelect(&bottomScreen);

    printf("\x1b[2J"); // Clear console
    printf("Printing on the bottom screen in a small window\n");

    consoleSelect(&topScreen);

    printf("Printing on the top screen\n");
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
