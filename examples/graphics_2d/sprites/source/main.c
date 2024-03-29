// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <nds.h>

// This is autogenerated from ball.png and ball.grit
#include "ball.h"

int main(int argc, char *argv[])
{
    videoSetMode(MODE_0_2D | DISPLAY_SPR_1D_LAYOUT | DISPLAY_SPR_ACTIVE);

    vramSetPrimaryBanks(VRAM_A_MAIN_SPRITE, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);

    oamInit(&oamMain, SpriteMapping_1D_32, false);

    oamEnable(&oamMain);

    // Allocate space for the tiles and copy them there
    u16 *gfx = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
    dmaCopy(ballTiles, gfx, ballTilesLen);

    // Copy palette
    dmaCopy(ballPal, SPRITE_PALETTE, ballPalLen);

    oamSet(&oamMain, 0,
           100, 50, // X, Y
           0, // Priority
           0, // Palette index
           SpriteSize_32x32, SpriteColorFormat_256Color, // Size, format
           gfx,  // Graphics offset
           -1, // Affine index
           false, // Double size
           false, // Hide
           false, false, // H flip, V flip
           false); // Mosaic

    oamSet(&oamMain, 1,
           150, 70, // X, Y
           0, // Priority
           0, // Palette index
           SpriteSize_32x32, SpriteColorFormat_256Color, // Size, format
           gfx,  // Graphics offset
           -1, // Affine index
           false, // Double size
           false, // Hide
           false, false, // H flip, V flip
           false); // Mosaic

    consoleDemoInit();

    printf("PAD:   Move sprite\n");
    printf("START: Exit to loader\n");

    int x = 150, y = 70;

    while (1)
    {
        swiWaitForVBlank();

        oamSetXY(&oamMain, 1, x, y);

        oamUpdate(&oamMain);

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
