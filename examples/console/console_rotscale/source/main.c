// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <nds.h>

#include <custom_font.h>

ConsoleFont customFont = {
    .gfx = (u16 *)custom_fontTiles,
    .pal = (u16 *)custom_fontPal,
    .numColors = 256,
    .bpp = 8,
    .asciiOffset = 0,
    .numChars = 128,
};

int main(int argc, char **argv)
{
    // Mode 5 supports extended rotation backgrounds
    videoSetMode(MODE_5_2D);

    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);

    // The console API assumes that every character uses 2 bytes of VRAM.
    // Regular tiled backgrounds use two bytes per tile. Affine backgrounds only
    // use one byte per tile, and extended rotation backgrounds use two. That's
    // why we need to use an extended rotation background.
    PrintConsole *console = consoleInit(NULL, 3, BgType_ExRotation, BgSize_ER_256x256,
                                        31, 0,
                                        true, // Main screen
                                        false); // Don't load graphics

    // Load our custom 8bpp font
    consoleSetFont(console, &customFont);

    int bg = console->bgId;

    consoleClear();

    for (int i = 0; i < 256; i++)
        printf("\x1b[%d;%dH%c", i / 32, i % 32, i);

    printf("ABXY:  Move rotation center\n");
    printf("PAD:   Scroll background\n");
    printf("L/R:   Rotate\n");
    printf("SELECT/START: Scale\n");

    int x = 0, y = 0;
    int cx = 0, cy = 0;

    int angle = 10;
    int scale = 1 << 8;

    while (1)
    {
        swiWaitForVBlank();

        // Update registers during the vertical blanking period to prevent
        // screen tearing.
        bgUpdate();

        scanKeys();

        u16 keys_held = keysHeld();

        if (keys_held & KEY_UP)
            y++;
        else if (keys_held & KEY_DOWN)
            y--;
        if (keys_held & KEY_LEFT)
            x++;
        else if (keys_held & KEY_RIGHT)
            x--;

        if (keys_held & KEY_L)
            angle -= 1 << 4;
        else if (keys_held & KEY_R)
            angle += 1 << 4;

        if (keys_held & KEY_SELECT)
            scale += 1 << 3;
        else if (keys_held & KEY_START)
            scale -= 1 << 3;

        if (keys_held & KEY_X)
            cy--;
        else if (keys_held & KEY_B)
            cy++;
        if (keys_held & KEY_Y)
            cx--;
        else if (keys_held & KEY_A)
            cx++;

        bgSetCenter(bg, cx, cy);
        bgSetRotateScale(bg, angle, scale, scale);
        bgSetScroll(bg, x, y);
    }

    return 0;
}
