// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

// To see the debug messages in no$gba, open the "Window" menu and click
// "TTY Debug Messages".
//
// To switch between ARM7 and ARM9 go to "Window" and press "ARM9 (Main CPU)" or
// "ARM7 (Sub CPU)".

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Redirect stderr to the no$gba debug console. By default both stdout and
    // stderr are redirected to the libnds console.
    consoleDebugInit(DebugDevice_NOCASH);

    printf("Check the debug console of\n"
           "no$gba of both ARM9 and ARM7\n"
           "to see the debug messages.\n"
           "\n"
           "\n"
           "Press START to exit");

    while (1)
    {
        fprintf(stderr, "ARM9 %%frame%%\n");

        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysHeld();
        if (keys & KEY_START)
            break;
    }

    return 0;
}
