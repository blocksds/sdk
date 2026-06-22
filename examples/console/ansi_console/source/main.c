// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

// Information about ANSI escape codes:
//
//     https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_(Control_Sequence_Introducer)_sequences
//
// Check the source code of the console in libnds to check which codes are
// supported and which ones are not: source/arm9/console.c

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Enable improved standard color handling. By default, libnds supports
    // a single color command format that doesn't follow the standard.
    consoleEnhancedColorHandler(NULL);

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

    // Move cursor to a specific location to start printing colored text
    printf("\x1b[14;4H");

    // Color commands are a list of numbers separated by ';'.
    //
    // - Command 0 resets all attributes to the default.
    // - Commands 30 to 37 are foreground colors.
    // - Commands 40 to 37 are background colors.
    // - Command 1 will make them brighter (intense).
    // - Command 22 will make them dimmer (normal intensity).
    // - Commands 90 to 97 are bright (intense) foreground colors.
    // - Commands 100 to 107 are bright (intense) background colors.
    //
    // Notes:
    // - The default console doesn't support background colors.
    // - libnds supports up to 3 commands in the same escape sequence.
    // - Colors: Black, Red, Green, Yellow, Blue, Magenta, Cyan, White

    char c = 'A';
    for (int color = 30; color <= 37; color++)
        printf("\x1b[1;%dm%c", color , c++);
    for (int color = 30; color <= 37; color++)
        printf("\x1b[22;%dm%c", color , c++);
    for (int color = 90; color <= 97; color++)
        printf("\x1b[%dm%c", color , c++);

    // Reset all color settings
    printf("\x1b[39m");

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
