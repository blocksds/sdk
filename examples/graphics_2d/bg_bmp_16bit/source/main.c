// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <nds.h>

// This is autogenerated from city.png and city.grit
#include "city.h"

int main(int argc, char *argv[])
{
    // Mode 5 lets you use layers 2 and 3 as a bitmap
    videoSetMode(MODE_5_2D);

    // The screen is 256x192 pixels. A 16-bit bitmap that size requires 96 KB
    // (256 x 192 x 2 / 1024). Each VRAM bank is 128 KB, so we need two banks to
    // have a double buffer setup.
    vramSetPrimaryBanks(VRAM_A_MAIN_BG_0x06000000, VRAM_B_MAIN_BG_0x06020000,
                        VRAM_C_LCD, VRAM_D_LCD);

    int bg = bgInit(2, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

    consoleDemoInit();

    printf("PAD:   Move square\n");
    printf("START: Exit to loader\n");

    int x = 20, y = 30;

    while (1)
    {
        swiWaitForVBlank();

        uint16_t *backbuffer = bgGetGfxPtr(bg);

        // Bitmap base 0 is at offset 0 (start of VRAM A) and bitmap base 8 is
        // at offset 128 KB (8 * 128 * 128 B) (start of VRAM B).
        if (bgGetMapBase(bg) == 8)
            bgSetMapBase(bg, 0);
        else
            bgSetMapBase(bg, 8);

        // Draw scene. Copy background image and draw a square on top of it.
        dmaCopy(cityBitmap, backbuffer, cityBitmapLen);
        for (int j = y; j < 50 + y; j++)
        {
            for (int i = x; i < 50 + x; i++)
            {
                // Invert all colors but leave the top bit as 1 so that the
                // image doesn't become transparent.
                backbuffer[256 * j + i] ^= 0x7FFF;
            }
        }

        scanKeys();
        u16 keys_held = keysHeld();

        if (keys_held & KEY_UP)
            y--;
        else if (keys_held & KEY_DOWN)
            y++;

        if (keys_held & KEY_LEFT)
            x--;
        else if (keys_held & KEY_RIGHT)
            x++;

        if (keys_held & KEY_START)
            break;
    }

    return 0;
}
