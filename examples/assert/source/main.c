// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

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

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    for (int i = 0; i < 10; i++)
        sassert(i < 5, "Error message");

    consoleDemoInit();

    // Clear console
    printf("\x1b[2J");

    printf("If you're reading this message,\n"
           "assertions are disabled.");

    printf("\x1b[23;0HPress START to exit to loader");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
