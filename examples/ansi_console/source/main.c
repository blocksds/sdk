// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

// Information about ANSI escape codes:
//
//     https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_(Control_Sequence_Introducer)_sequences
// Check the source code of the console in libnds to check which codes are
// supported and which ones are not: source/arm9/console.c

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Clear console: [2J
    printf("\x1b[2J");

    // Set cursor coordinates: [y;xH
    printf("\x1b[8;10HHello World!");

    // Move cursor up: [deltayA
    printf("\x1b[8ALine 0");

    // Move cursor left: [deltaxD
    printf("\x1b[28DColumn 0");

    // Move cursor down: [deltayB
    printf("\x1b[19BLine 19");

    // Move cursor right: [deltaxC
    printf("\x1b[5CColumn 20");

    // Print colored text
    printf("\x1b[14;4H");

    // Colors (30 to 37): Black, Red, Green, Yellow, BLue, Magenta, Cyan, White
    // Setting intensity to 1 will make them brighter.
    char c = 'A';
    for (int intensity = 0; intensity < 2; intensity++)
        for (int color = 30; color < 38; color++)
            printf("\x1b[%d;%dm%c", color, intensity, c++);

    // Reset color to white
    printf("\x1b[39;0m");

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
