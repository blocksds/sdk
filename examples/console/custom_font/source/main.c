// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <nds.h>

#include <anuvverbubbla_8x8.h>
#include <charmap_cellphone.h>
#include <charmap_futuristic.h>

// Size of a color in bytes
const size_t size_color = 2;

// Size of a font character in 4 and 8 BPP modes in bytes
const size_t size_char_4bpp = (8 * 8) / 2;
const size_t size_char_8bpp = 8 * 8;

ConsoleFont font_anuvverbubbla= {
    .gfx = anuvverbubbla_8x8Tiles,
    .pal = anuvverbubbla_8x8Pal,
    .numColors = anuvverbubbla_8x8PalLen / size_color,
    .bpp = 8,
    .asciiOffset = 32,
    .numChars = anuvverbubbla_8x8TilesLen / size_char_8bpp,
};

ConsoleFont font_cellphone = {
    .gfx = charmap_cellphoneTiles,
    .pal = charmap_cellphonePal,
    .numColors = charmap_cellphonePalLen / size_color,
    .bpp = 4,
    .asciiOffset = 32,
    .numChars = charmap_cellphoneTilesLen / size_char_4bpp,
};

ConsoleFont font_futuristic = {
    .gfx = charmap_futuristicTiles,
    .pal = charmap_futuristicPal,
    .numColors = charmap_futuristicPalLen / size_color,
    .bpp = 4,
    .asciiOffset = 32,
    .numChars = charmap_futuristicTilesLen / size_char_4bpp,
};

int main(int argc, char **argv)
{
    PrintConsole main_console;

    PrintConsole sub_console1;
    PrintConsole sub_console2;

    videoSetMode(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);

    videoSetModeSub(MODE_0_2D);
    vramSetBankC(VRAM_C_SUB_BG);

    setBackdropColorSub(RGB15(5, 5, 5));

    // Initialize the consoles with default libnds settings, but don't load
    // graphics yet so that it doesn't load the default libnds font and
    // palettes.

    consoleInitEx(&main_console,
                  1,                // Background layer
                  BgType_Text8bpp,  // 8 BPP mode
                  BgSize_T_256x256, // Size of the background layer
                  0,                // Use map base 0 (they can't be shared)
                  3,                // Use tile base 3
                  0,                // Use palette index 0 (256 color palette)
                  0,                // Start from character 0 of the tile base
                  true,             // Main screen
                  false);           // Don't load graphics

    // As both fonts only have the characters from ASCII 32 to 127, which isn't
    // a lot of tiles, we can load both of them in the same tile slot. The first
    // font can start at character 0, the second one can start at character 128.
    // This lets us optimize VRAM usage.
    consoleInitEx(&sub_console1,
                  0,                // Background layer
                  BgType_Text4bpp,  // 4 BPP mode
                  BgSize_T_256x256, // Size of the background layer
                  0,                // Use map base 0 (they can't be shared)
                  3,                // Use tile base 3
                  4,                // Use palette index 4 (16 color palettes)
                  0,                // Start from character 0 of the tile base
                  false,            // Sub screen
                  false);           // Don't load graphics

    consoleInitEx(&sub_console2,
                  2,                // Background layer
                  BgType_Text4bpp,  // 4 BPP mode
                  BgSize_T_256x256, // Size of the background layer
                  1,                // Use map base 1 (they can't be shared)
                  3,                // Use tile base 3
                  8,                // Use palette index 8 (16 color palettes)
                  128,              // Start from character 128 of the tile base
                  false,            // Sub screen
                  false);           // Don't load graphics

    // Now, load the custom graphics

    consoleSetFont(&main_console, &font_anuvverbubbla);

    consoleSetFont(&sub_console1, &font_cellphone);
    consoleSetFont(&sub_console2, &font_futuristic);

    // Print some text to test the consoles

    consoleSelect(&main_console);
    printf("THIS IS A 8 BPP FONT:\n");
    for (int i = 0; i < 128; i++)
        printf("%c", i);

    consoleSelect(&sub_console1);
    printf("This is a 4 BPP font:\n");
    for (int i = 0; i < 128; i++)
        printf("%c", i);

    consoleSelect(&sub_console2);
    consoleSetCursor(&sub_console2, 0, 8);
    printf("This is a 4 BPP font too:\n");
    for (int i = 0; i < 128; i++)
        printf("%c", i);

    consoleSetCursor(&sub_console2, 0, 23);
    printf("Press START to exit to loader");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
