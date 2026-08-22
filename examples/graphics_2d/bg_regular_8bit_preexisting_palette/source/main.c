// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

// This example shows how to tell grit to use a predefined palette as base when
// converting images. This can be useful for some effects that involve modifying
// palettes, as you need to know where is each color. For example:
//
// - In a palette cycle effect (this example) you need to know that all colors
//   are arranged in a hue cycle.
// - In some games, like fighting games, you may want to change the palette of a
//   character if both players have selected the same character.
//
// The hardest part is to create the image with the palette. grit needs to be
// able to load the colors in the right order. Use an image creation program
// that allows you to edit the palette if possible.

#include <nds.h>

#include "grit/rainbow_image.h"
#include "grit/shared_data.h"

int main(int argc, char *argv[])
{
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_LCD, VRAM_C_SUB_BG, VRAM_D_LCD);

    // Setup the background

    int bg = bgInitHidden(2, BgType_Text8bpp, BgSize_T_256x256, 0, 1);

    memcpy(bgGetMapPtr(bg), rainbow_imageMap, rainbow_imageMapLen);
    memcpy(bgGetGfxPtr(bg), rainbow_imageTiles, rainbow_imageTilesLen);
    memcpy(BG_PALETTE, shared_dataPal, shared_dataPalLen);

    // Show it

    bgShow(bg);

    int x = 0, y = 0;
    int color_shift = 0;

    while (1)
    {
        swiWaitForVBlank();

        const int num_colors = shared_dataPalLen / sizeof(u16);
        for (int i = 0; i < num_colors; i++)
            BG_PALETTE[i] = ((u16*)shared_dataPal)[(i + color_shift) & (num_colors - 1)];
        color_shift--;

        bgSetScroll(bg, x, y);

        bgUpdate();

        scanKeys();

        u16 keys_held = keysHeld();

        if (keys_held & KEY_START)
            break;
    }

    return 0;
}
