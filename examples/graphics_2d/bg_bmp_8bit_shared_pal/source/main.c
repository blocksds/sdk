// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024-2026

// This example shows how to convert two different backgrounds to 8-bit bitmap
// mode. Each of them has their own bitmap, but they share the same palette.
//
// Be careful with your original images, they combined palette must have at most
// 256 colors.

#include <nds.h>

#include "grit/manga_bg_01.h"
#include "grit/manga_bg_07.h"
#include "grit/shared_data.h"

int main(int argc, char *argv[])
{
    // Mode 5 lets you use layers 2 and 3 as a bitmap
    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_5_2D);

    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_LCD, VRAM_C_SUB_BG, VRAM_D_LCD);

    // Setup one background on each engine

    int bg = bgInitHidden(2, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    int bgsub = bgInitHiddenSub(2, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

    // Load one bitmap to the main engines

    memcpy(bgGetGfxPtr(bg), manga_bg_01Bitmap, manga_bg_01BitmapLen);

    // Load the other bitmap to the sub engine

    memcpy(bgGetGfxPtr(bgsub), manga_bg_07Bitmap, manga_bg_07BitmapLen);

    // Load the same palette to the main and sub engines

    memcpy(BG_PALETTE, shared_dataPal, shared_dataPalLen);
    memcpy(BG_PALETTE_SUB, shared_dataPal, shared_dataPalLen);

    // Show the backgrounds

    bgShow(bg);
    bgShow(bgsub);

    int x = 0, y = 0;

    while (1)
    {
        swiWaitForVBlank();

        bgSetScroll(bg, x, y);
        bgSetScroll(bgsub, x, y);

        bgUpdate();

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
