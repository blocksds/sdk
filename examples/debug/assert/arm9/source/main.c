// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

// It is required to use the debug build of libnds for this to work. In your
// Makefile you will need to link "-lnds9d" instead of "-lnds9". Also, all
// asserts will be compiled out if NDEBUG is defined.
//
// Debug build:
//
//     LIBS     := -lnds9d
//
// Release build:
//
//     LIBS     := -lnds9
//     DEFINES  := -DNDEBUG
//
// You need to do the same for the ARM7 build (replace "-lnds7" by "-lnds7d").

#include <assert.h>
#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    defaultExceptionHandler();

    consoleDemoInit();

    printf("A: Trigger assert on ARM9\n");
    printf("B: Trigger assert on ARM7\n\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        uint16_t keys = keysHeld();

        if (keys & KEY_A)
        {
            printf("Causing assertion on ARM9\n\n");

            timerStart(10, ClockDivider_1, TIMER_FREQ(5000), NULL);

            break;
        }

        if (keys & KEY_B)
        {
            printf("Causing assertion on ARM7\n\n");

            fifoSendValue32(FIFO_USER_07, 0);
            break;
        }
    }

    printf("If you're reading this message,\n"
           "assertions are disabled.");

    consoleSetCursor(NULL, 0, 23);
    printf("Press START to exit to loader");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
