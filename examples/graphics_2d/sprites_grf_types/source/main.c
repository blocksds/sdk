// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// This example shows how to load any type of sprite from GRF files (16-color,
// 256-color and bitmap sprites). It also shows how to use 256 color extended
// palettes.

#include <filesystem.h>
#include <nds.h>

#include "sprite.h"

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

static void *gfx16Main, *gfx16Sub;
static void *gfx256Main, *gfx256Sub;
static void *gfx256ExtMain, *gfx256ExtSub;
static void *gfxBmpMain, *gfxBmpSub;

void load_sprites_16_colors(void)
{
    int ret;
    SpriteSize size;
    SpriteColorFormat format;

    int palette_index = 4;

    ret = oamLoadGfxGrf(&oamMain, "grit/tiles_16_png.grf", palette_index,
                        &gfx16Main, &size, &format);
    if (ret != 0)
    {
        printf("Failed to load gfx16Main: %d\n", ret);
        wait_forever();
    }

    oamSet(&oamMain,
        10, // Index
        0, 120, // X, Y
        0, // Priority
        palette_index, // Palette index
        size, format, gfx16Main,
        -1, // Affine index
        false, // Double size
        false, // Hide
        false, false, // H flip, V flip
        false); // Mosaic

    ret = oamLoadGfxGrf(&oamSub, "grit/tiles_16_png.grf", palette_index,
                        &gfx16Sub, &size, &format);
    if (ret != 0)
    {
        printf("Failed to load gfx16Sub: %d\n", ret);
        wait_forever();
    }

    oamSet(&oamSub,
        10, // Index
        0, 120, // X, Y
        0, // Priority
        palette_index, // Palette index
        size, format, gfx16Sub,
        -1, false, false, false, false, false);
}

void unload_sprites_16_colors(void)
{
    oamFreeGfx(&oamMain, gfx16Main);
    oamFreeGfx(&oamSub, gfx16Sub);
}

void load_sprites_256_colors(void)
{
    int ret;
    SpriteSize size;
    SpriteColorFormat format;

    ret = oamLoadGfxGrf(&oamMain, "grit/tiles_256_png.grf", 0, &gfx256Main,
                        &size, &format);
    if (ret != 0)
    {
        printf("Failed to load sprite graphics: %d\n", ret);
        wait_forever();
    }

    oamSet(&oamMain,
        20, // Index
        64, 120, // X, Y
        0, // Priority
        0, // Palette index
        size, format, gfx256Main,
        -1, false, false, false, false, false);

    ret = oamLoadGfxGrf(&oamSub, "grit/tiles_256_png.grf", 0, &gfx256Sub,
                        &size, &format);
    if (ret != 0)
    {
        printf("Failed to load sprite graphics: %d\n", ret);
        wait_forever();
    }

    oamSet(&oamSub,
        20, // Index
        64, 120, // X, Y
        0, // Priority
        0, // Palette index
        size, format, gfx256Sub,
        -1, false, false, false, false, false);
}

void unload_sprites_256_colors(void)
{
    oamFreeGfx(&oamMain, gfx256Main);
    oamFreeGfx(&oamSub, gfx256Sub);
}

void load_sprites_256_colors_extended_palettes(void)
{
    int ret;
    SpriteSize size;
    SpriteColorFormat format;

    int palette_index = 4;

    ret = oamLoadGfxGrf(&oamMain, "grit/tiles_256_png.grf", palette_index,
                        &gfx256ExtMain, &size, &format);
    if (ret != 0)
    {
        printf("Failed to load sprite graphics: %d\n", ret);
        wait_forever();
    }

    oamSet(&oamMain,
        30, // Index
        128, 120, // X, Y
        0, // Priority
        palette_index, // Palette index
        size, format, gfx256ExtMain,
        -1, false, false, false, false, false);

    ret = oamLoadGfxGrf(&oamSub, "grit/tiles_256_png.grf", palette_index,
                        &gfx256ExtSub, &size, &format);
    if (ret != 0)
    {
        printf("Failed to load sprite graphics: %d\n", ret);
        wait_forever();
    }

    oamSet(&oamSub,
        30, // Index
        128, 120, // X, Y
        0, // Priority
        palette_index, // Palette index
        size, format, gfx256ExtSub,
        -1, false, false, false, false, false);
}

void unload_sprites_256_colors_extended_palettes(void)
{
    oamFreeGfx(&oamMain, gfx256ExtMain);
    oamFreeGfx(&oamSub, gfx256ExtSub);
}

void load_sprites_bitmap(void)
{
    int ret;
    SpriteSize size;
    SpriteColorFormat format;

    ret = oamLoadGfxGrf(&oamMain, "grit/bitmap_png.grf", 0,
                        &gfxBmpMain, &size, &format);
    if (ret != 0)
    {
        printf("Failed to load sprite graphics: %d\n", ret);
        wait_forever();
    }

    oamSet(&oamMain,
        40, // Index
        192, 120, // X, Y
        0, // Priority
        8, // Palette index, but it is the alpha value of bitmap sprites
        size, format, gfxBmpMain,
        -1, false, false, false, false, false);

    ret = oamLoadGfxGrf(&oamSub, "grit/bitmap_png.grf", 0,
                        &gfxBmpSub, &size, &format);
    if (ret != 0)
    {
        printf("Failed to load sprite graphics: %d\n", ret);
        wait_forever();
    }

    oamSet(&oamSub,
        40, // Index
        192, 120, // X, Y
        0, // Priority
        8, // Palette index, but it is the alpha value of bitmap sprites
        size, format, gfxBmpSub,
        -1, false, false, false, false, false);
}

void unload_sprites_bitmap(void)
{
    oamFreeGfx(&oamMain, gfxBmpMain);
    oamFreeGfx(&oamSub, gfxBmpSub);
}

int main(int argc, char *argv[])
{
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    // VRAM_C is used by consoleDemoInit()
    vramSetPrimaryBanks(VRAM_A_MAIN_SPRITE, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_SUB_SPRITE);

    consoleDemoInit();

    printf("Initializing NitroFS...\n");

    if (!nitroFSInit(NULL))
    {
        perror("nitroFSInit()");
        wait_forever();
    }

    // First scene
    // ===========

    // In this first scene, extended palettes are disabled. This means that both
    // 16-color palettes and the 256-color share the same space. We can't easily
    // load both types of sprites, so we only load 256-color and bitmap sprites.

    // Unmap banks used for extended palettes
    vramSetBankE(VRAM_E_LCD);
    vramSetBankF(VRAM_F_LCD);
    vramSetBankG(VRAM_G_LCD);
    vramSetBankH(VRAM_H_LCD);
    vramSetBankI(VRAM_I_LCD);

    // Initialize sprites without extended palettes
    oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false);
    oamInit(&oamSub, SpriteMapping_Bmp_1D_128, false);

    // 16 and 256 color sprites share the same palette when extended palettes
    // aren't enabled.
    load_sprites_256_colors();
    load_sprites_bitmap();

    consoleClear();
    printf("Extended palettes: OFF\n");
    printf("\n");
    printf("Sprites shown:\n");
    printf("- 256 colors (regular)\n");
    printf("- Bitmap\n");
    printf("\n");
    printf("\n");
    printf("A: Next scene\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("        256               Bmp\n");

    while (1)
    {
        swiWaitForVBlank();

        oamUpdate(&oamMain);
        oamUpdate(&oamSub);

        scanKeys();

        u16 keys_held = keysHeld();

        if (keys_held & KEY_A)
            break;
    }

    // Free graphics that we have allocated before. Note that this isn't
    // strictly needed in this example because oamInit() resets all the
    // allocation state, but it's added in this example to show how to do it in
    // a regular game.
    unload_sprites_256_colors();
    unload_sprites_bitmap();

    // Second scene
    // ============

    // In this first scene, extended palettes are enabled. 16-color palettes use
    // the regular palette memory, and 256-color palettes use only extended
    // palettes. This means we can easily load both types of sprites easily (as
    // well as bitmap sprites).

    // Map some VRAM to be used as extended palettes
    vramSetBankF(VRAM_F_SPRITE_EXT_PALETTE);
    vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);

    // Re-initialize sprites with extended palettes active
    oamInit(&oamMain, SpriteMapping_Bmp_1D_128, true);
    oamInit(&oamSub, SpriteMapping_Bmp_1D_128, true);

    // When using extended palettes, 16-color sprites use the regular sprite
    // palette VRAM, and 256-color sprites use extended palette VRAM. They don't
    // overlap.
    load_sprites_16_colors();
    load_sprites_256_colors_extended_palettes();
    load_sprites_bitmap();

    consoleClear();
    printf("Extended palettes: ON\n");
    printf("\n");
    printf("Sprites shown:\n");
    printf("- 16 colors\n");
    printf("- 256 colors (extended)\n");
    printf("- Bitmap\n");
    printf("\n");
    printf("\n");
    printf("START: Exit to loader\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("16              256-Ext   Bmp\n");

    while (1)
    {
        swiWaitForVBlank();

        oamUpdate(&oamMain);
        oamUpdate(&oamSub);

        scanKeys();

        u16 keys_held = keysHeld();

        if (keys_held & KEY_START)
            break;
    }

    unload_sprites_16_colors();
    unload_sprites_256_colors_extended_palettes();
    unload_sprites_bitmap();

    return 0;
}
