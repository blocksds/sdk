// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024-2025

#include <nds.h>

// This is autogenerated from manga_bg.png and manga_bg.grit
#include "manga_bg.h"

int main(int argc, char *argv[])
{
    // consoleDemoInit() uses VRAM_C to store the console graphics, but we need
    // banks A, B, C and D for the bitmap. We need to initialize the sub screen
    // and do a non-default console init.
    videoSetModeSub(MODE_0_2D);
    vramSetBankH(VRAM_H_SUB_BG_0x06200000);
    consoleInit(NULL, 1, BgType_Text4bpp, BgSize_T_256x256, 4, 0, false, true);

    // Now, setup the main engine to display the bitmap

    // Mode 6 lets you use layers 2 as a large bitmap
    videoSetMode(MODE_6_2D);

    // Video mode 6 is designed to display a large bitmap stored in all primary
    // VRAM banks: 128 KB * 4 = 512 KB in total.
    vramSetPrimaryBanks(VRAM_A_MAIN_BG_0x06000000,
                        VRAM_B_MAIN_BG_0x06020000,
                        VRAM_C_MAIN_BG_0x06040000,
                        VRAM_D_MAIN_BG_0x06060000);

    // Valid sizes are 1024x512 and 512x1024
    int bg = bgInit(2, BgType_Bmp8, BgSize_B8_1024x512, 0, 0);

    printf("PAD:   Scroll\n");
    printf("START: Exit to loader\n");

    int x = 0, y = 0;

    // Load the palette and data
    dmaCopy(manga_bgPal, BG_PALETTE, manga_bgPalLen);
    dmaCopy(manga_bgBitmap, bgGetGfxPtr(bg), manga_bgBitmapLen);

    while (1)
    {
        swiWaitForVBlank();

        bgSetScroll(bg, x, y);
        bgUpdate();

        scanKeys();
        u16 keys_held = keysHeld();

        if (keys_held & KEY_UP)
            y -= 4;
        else if (keys_held & KEY_DOWN)
            y += 4;

        if (keys_held & KEY_LEFT)
            x -= 4;
        else if (keys_held & KEY_RIGHT)
            x += 4;

        if (keys_held & KEY_START)
            break;
    }

    return 0;
}
