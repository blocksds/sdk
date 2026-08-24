// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024-2026

#include <nds.h>

#include "ptexconv/forest_png_chr_bin.h"
#include "ptexconv/forest_png_pal_bin.h"
#include "ptexconv/forest_png_scr_bin.h"

int main(int argc, char *argv[])
{
    videoSetMode(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);

    int bg = bgInitHidden(0, BgType_Text4bpp, BgSize_T_256x256, 0, 1);

    memcpy(bgGetGfxPtr(bg), forest_png_chr_bin, forest_png_chr_bin_size);
    memcpy(bgGetMapPtr(bg), forest_png_scr_bin, forest_png_scr_bin_size);
    memcpy(BG_PALETTE, forest_png_pal_bin, forest_png_pal_bin_size);

    bgShow(bg);

    consoleDemoInit();

    printf("PAD:   Scroll background\n");
    printf("START: Exit to loader\n");

    int x = 0, y = 0;

    while (1)
    {
        swiWaitForVBlank();

        bgSetScroll(bg, x, y);

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
