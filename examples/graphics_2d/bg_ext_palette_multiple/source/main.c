// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024-2026

#include <nds.h>

// This example shows how to use multiple extended palettes in a single image,
// giving you more flexibility about the amount of colors that you can display
// in a single tiled background.

#include "ptexconv/photo_png_chr_bin.h"
#include "ptexconv/photo_png_pal_bin.h"
#include "ptexconv/photo_png_scr_bin.h"

int main(int argc, char *argv[])
{
    videoSetMode(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);

    // Let the CPU access VRAM E to copy the extended palettes. Extended
    // palette VRAM can't be accessed by the CPU (background VRAM can be
    // accessed even without mapping it as LCD).
    vramSetBankE(VRAM_E_LCD);

    // Enable extended palettes for backgrounds for the main engine
    bgExtPaletteEnable();

    // Main engine background

    int bg0 = bgInitHidden(0, BgType_Text8bpp, BgSize_T_256x256, 0, 1);

    memcpy(bgGetGfxPtr(bg0), photo_png_chr_bin, photo_png_chr_bin_size);
    memcpy(bgGetMapPtr(bg0), photo_png_scr_bin, photo_png_scr_bin_size);
    memcpy(&VRAM_E_EXT_PALETTE[0][0], photo_png_pal_bin, photo_png_pal_bin_size);

    // Setup VRAM as extended palette VRAM

    vramSetBankE(VRAM_E_BG_EXT_PALETTE);

    // Display background when it's ready

    bgShow(bg0);

    consoleDemoInit();
    printf("PAD:   Scroll background\n");
    printf("START: Exit to loader\n");

    int x = 0, y = 0;

    while (1)
    {
        swiWaitForVBlank();

        bgUpdate();

        bgSetScroll(bg0, x, y);

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

        if (keys_held & KEY_START)
            break;
    }

    return 0;
}
