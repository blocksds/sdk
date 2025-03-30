// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <filesystem.h>
#include <nds.h>

__attribute__((noreturn)) void wait_forever(void)
{
    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysHeld() & KEY_START)
            exit(1);
    }
}

int main(int argc, char *argv[])
{
    videoSetMode(MODE_0_2D);

    vramSetPrimaryBanks(VRAM_A_MAIN_SPRITE, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);

    consoleDemoInit();

    printf("Initializing NitroFS...\n");

    if (!nitroFSInit(NULL))
    {
        perror("nitroFSInit()");
        wait_forever();
    }

    oamInit(&oamMain, SpriteMapping_1D_32, false);

    oamEnable(&oamMain);

    u16 *gfx;
    {
        void *gfxDst = NULL;
        void *palDst = NULL;
        size_t gfxSize, palSize;
        GRFHeader header = { 0 };
        GRFError err = grfLoadPath("grit/statue_png.grf", &header,
                                   &gfxDst, &gfxSize, NULL, NULL,
                                   &palDst, &palSize);
        if (err != GRF_NO_ERROR)
        {
            printf("Couldn't load GRF file: %d", err);
            wait_forever();
        }

        if (gfxDst == NULL)
        {
            printf("No graphics found in GRF file");
            wait_forever();
        }

        if (palDst == NULL)
        {
            printf("No palette found in GRF file");
            wait_forever();
        }

        if (header.gfxAttr != 8)
        {
            printf("Invalid format in GRF file");
            wait_forever();
        }

        // Flush cache so that we can use DMA to copy the data to VRAM
        DC_FlushAll();

        // Allocate space for the tiles and copy them there
        gfx = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_256Color);
        dmaCopy(gfxDst, gfx, gfxSize);

        // Copy palette
        dmaCopy(palDst, SPRITE_PALETTE, palSize);

        free(gfxDst);
        free(palDst);
    }

    oamSet(&oamMain, 0,
           100, 50, // X, Y
           0, // Priority
           0, // Palette index
           SpriteSize_64x64, SpriteColorFormat_256Color, // Size, format
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
           SpriteSize_64x64, SpriteColorFormat_256Color, // Size, format
           gfx,  // Graphics offset
           -1, // Affine index
           false, // Double size
           false, // Hide
           false, false, // H flip, V flip
           false); // Mosaic

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

    oamFreeGfx(&oamMain, gfx);

    return 0;
}
