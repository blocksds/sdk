// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// Information about ANSI escape codes:
//
//     https://en.wikipedia.org/wiki/ANSI_escape_code#CSI_(Control_Sequence_Introducer)_sequences
// Check the source code of the console in libnds to check which codes are
// supported and which ones are not: source/arm9/console.c

#include <stdio.h>

#include <nds.h>

#include <custom_font.h>

ConsoleFont customFont = {
    .gfx = (u16 *)custom_fontTiles,
    .pal = NULL,
    .numColors = 0,
    .bpp = 1,
    .asciiOffset = 0,
    .numChars = 128,
    .convertSingleColor = true
};

int main(int argc, char **argv)
{
    PrintConsole *console = consoleDemoInit();

    consoleSetFont(console, &customFont);

    // Clear console: [2J
    printf("\x1b[2J");

    for (int i = 0; i < 256; i++)
        printf("\x1b[%d;%dH%c", i/32, i%32, i);

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
