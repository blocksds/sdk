// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

// There are two exception handlers in libnds:
//
// - The debug exception handler is setup by default in debug builds of libnds,
//   and it prints a lot of information to the screen (like the address where
//   the crash happened and the values of the CPU registers at that point).
//
// - The release exception handler only prints a minimal error message.
//
// If you want to use the debug handler in release builds, just call
// defaultExceptionHandler() in your code.
//
// In order to use a debug build of libnds, link to "-lnds9d" in your Makefile
// instead of "-lnds9".
//
// This example links to the debug version of libnds to show the debug exception
// handler, but you can switch to the release version to see that handler too.

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    // As explained above, this isn't required in debug builds, only in release
    // builds. In debug builds this is called before reaching main(). In release
    // builds, this replaces the minimal release exception handler.
    defaultExceptionHandler();
    //releaseExceptionHandler();

    consoleDemoInit();

    printf("A: Trigger exception on ARM9\n");
    printf("B: Trigger exception on ARM7\n\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        uint16_t keys = keysHeld();

        if (keys & KEY_A)
        {
            printf("Causing exception on ARM9\n\n");

            // Write data to an unmapped address to cause an exception
            // (The compiler can detect NULL being used here)
            *(uint32_t *)(0x00004000) = 0xDEAD;
            break;
        }

        if (keys & KEY_B)
        {
            printf("Causing exception on ARM7\n\n");

            fifoSendValue32(FIFO_USER_07, 0);
            break;
        }
    }

    printf("Exception handling failed!\n");
    printf("Is this in an emulator?\n");
    printf("\n");
    printf("Press START to exit\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
