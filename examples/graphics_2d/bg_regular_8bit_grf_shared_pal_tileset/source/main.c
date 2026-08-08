// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024-2026

// This example shows how to convert two different backgrounds to 8-bit tiled
// mode. Each of them has their tile map, but they share the same palette and
// tileset.
//
// You can use this to save space in your final application, or to display more
// than one background on the same screen using less VRAM.

#include <nds.h>
#include <filesystem.h>

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
    consoleDemoInit();
    printf("Initializing NitroFS...\n");

    if (!nitroFSInit(NULL))
    {
        perror("nitroFSInit()");
        wait_forever();
    }

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_LCD, VRAM_C_SUB_BG, VRAM_D_LCD);

    // Setup one background on each engine

    int bg = bgInitHidden(2, BgType_Text8bpp, BgSize_T_512x256, 0, 1);
    int bgsub = bgInitHiddenSub(2, BgType_Text8bpp, BgSize_T_256x256, 0, 1);

    // Load different tile maps to the main and sub engines

    {
        void *mapData = NULL;
        size_t mapSize;

        GRFHeader header = { 0 };
        GRFError err = grfLoadPath("grit/forest.grf", &header, NULL, NULL,
                                   &mapData, &mapSize, NULL, NULL);
        if (err != GRF_NO_ERROR)
        {
            consoleDemoInit();
            printf("grferr (sub): %d\n", err);
            wait_forever();
        }

        memcpy(bgGetMapPtr(bgsub), mapData, mapSize);

        free(mapData);
    }

    {
        void *mapData = NULL;
        size_t mapSize;

        GRFHeader header = { 0 };
        GRFError err = grfLoadPath("grit/forest_town.grf", &header, NULL, NULL,
                                   &mapData, &mapSize, NULL, NULL);
        if (err != GRF_NO_ERROR)
        {
            consoleDemoInit();
            printf("grferr (main): %d\n", err);
            wait_forever();
        }

        memcpy(bgGetMapPtr(bg), mapData, mapSize);

        free(mapData);
    }

    // Load shared data

    {
        void *gfxData = NULL;
        void *palData = NULL;
        size_t gfxSize, palSize;

        GRFHeader header = { 0 };
        GRFError err = grfLoadPath("grit/shared_data.grf", &header, &gfxData, &gfxSize,
                                   NULL, NULL, &palData, &palSize);
        if (err != GRF_NO_ERROR)
        {
            consoleDemoInit();
            printf("grferr (shared): %d\n", err);
            wait_forever();
        }

        // Load the same tileset to the main and sub engines

        memcpy(bgGetGfxPtr(bg), gfxData, gfxSize);
        memcpy(bgGetGfxPtr(bgsub), gfxData, gfxSize);

        // Load the same palette to the main and sub engines

        memcpy(BG_PALETTE, palData, palSize);
        memcpy(BG_PALETTE_SUB, palData, palSize);

        free(gfxData);
        free(palData);
    }

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
