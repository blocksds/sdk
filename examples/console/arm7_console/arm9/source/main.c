// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// This example sets up two consoles. One is assigned to the top screen and it's
// used by the ARM9 as usual. The other one is assigned to the top screen and
// it's setup so that any text sent by the ARM7 is printed there by the ARM9.

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

    // Setup ARM7 console
    // ------------------

    consoleSelect(&bottomScreen);

    // Use bright green
    consoleSetColor(&bottomScreen, CONSOLE_LIGHT_GREEN);

    consoleArm7Setup(&bottomScreen, 1024);

    // Switch back to the ARM9 console
    // -------------------------------

    consoleSelect(&topScreen);

    printf("Top screen: ARM9\n");
    printf("Bottom screen: ARM7\n");
    printf("\n");
    printf("\n");
    printf("Press START to exit to loader\n");

    int count = 0;

    while (1)
    {
        swiWaitForVBlank();

        consoleSetCursor(&topScreen, 0, 8);
        printf("ARM9 counter: %03d", count);

        count++;
        if (count == 1000)
            count = 0;

        scanKeys();

        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
