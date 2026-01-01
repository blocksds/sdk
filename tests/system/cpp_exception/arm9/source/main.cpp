// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

// This tries to cause an unhandled C++ exception, which should be managed by
// the function passed to std::set_terminate().

#include <stdlib.h>

#include <exception>
#include <vector>

#include <nds.h>

int main(int argc, char *argv[])
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
    consoleSetColor(&bottomScreen, CONSOLE_LIGHT_GRAY);

    consoleArm7Setup(&bottomScreen, 2048);

    // Switch back to the ARM9 console
    // -------------------------------

    consoleSelect(&topScreen);

    std::set_terminate([]()
    {
        printf("Unhandled exception!\nExitting program...");

        for (int i = 0; i < 5; i++)
        {
            printf(".");
            for (int j = 0; j < 60; j++)
                swiWaitForVBlank();
        }

        exit(0);
    });

    printf("===== ARM9 =====\n");
    printf("A: Trigger C++ exception\n");
    printf("START: Exit to loader\n\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        uint16_t keys = keysHeld();

        if (keys & KEY_A)
        {
            printf("Triggering exception...\n");

            std::vector<int> vec(1024  * 1024 * 1024, 100);

            printf("This shouldn't be reached.\n");
        }

        if (keys & KEY_START)
            break;
    }
}
