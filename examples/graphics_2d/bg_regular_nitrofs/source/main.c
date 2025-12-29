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

    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);

    consoleDemoInit();

    printf("Initializing NitroFS...\n");

    if (!nitroFSInit(NULL))
    {
        perror("nitroFSInit()");
        wait_forever();
    }

    int bg;
    {
        void *gfxDst = NULL;
        size_t gfxSize, mapSize, palSize;
        void *mapDst = NULL;
        void *palDst = NULL;
        GRFHeader header = { 0 };
        GRFError err = grfLoadPath("grit/forest_town_png.grf", &header,
                                   &gfxDst, &gfxSize, &mapDst, &mapSize,
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

        if (mapDst == NULL)
        {
            printf("No map found in GRF file");
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

        bg = bgInit(0, BgType_Text8bpp, BgSize_T_512x256, 0, 1);

        // Flush cache so that we can use DMA to copy the data to VRAM
        DC_FlushAll();

        dmaCopy(gfxDst, bgGetGfxPtr(bg), gfxSize);
        dmaCopy(mapDst, bgGetMapPtr(bg), mapSize);
        dmaCopy(palDst, BG_PALETTE, palSize);
    }

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
