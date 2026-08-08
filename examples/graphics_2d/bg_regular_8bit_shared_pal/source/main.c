// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024-2026

// This example shows how to convert two different backgrounds to 8-bit tiled
// mode. Each of them has their own tile set and tile map, but they share the
// same palette.
//
// This is useful if you want to display them on the same screen in different
// layers (without using extended background palettes).

#include <nds.h>

#include "grit/forest.h"
#include "grit/forest_town.h"
#include "grit/shared_data.h"

int main(int argc, char *argv[])
{
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_LCD, VRAM_C_SUB_BG, VRAM_D_LCD);

    // Setup one background on each engine

    int bg = bgInitHidden(2, BgType_Text8bpp, BgSize_T_512x256, 0, 1);
    int bgsub = bgInitHiddenSub(2, BgType_Text8bpp, BgSize_T_256x256, 0, 1);

    // Load different tile maps to the main and sub engines

    memcpy(bgGetGfxPtr(bg), forest_townTiles, forest_townTilesLen);
    memcpy(bgGetMapPtr(bg), forest_townMap, forest_townMapLen);

    // Load the other tile set and tile map to the sub engine

    memcpy(bgGetGfxPtr(bgsub), forestTiles, forestTilesLen);
    memcpy(bgGetMapPtr(bgsub), forestMap, forestMapLen);

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
