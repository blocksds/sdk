// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

// This example shows how to load any type of background from GRF files. This
// example isn't as useful as the one that loads sprites from GRF files:
//
// - libnds doesn't have a background data allocator. You need to specify the
//   map base and tile base manually.
//
// - You need to specify the background video mode manually, which changes which
//   background layers can have

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

void wait_press_a(void)
{
    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        if (keysDown() & KEY_A)
            break;
    }
}

int backgroundLoadGrf(const char *path, int layer, BgType type, int map_base, int tile_base)
{
    int ret = 0;

    void *mapData = NULL;
    void *gfxData = NULL;
    void *palData = NULL;
    size_t mapSize, gfxSize, palSize;

    GRFHeader header = { 0 };
    GRFError err = grfLoadPath(path, &header, &gfxData, &gfxSize,
                               &mapData, &mapSize, &palData, &palSize);
    if (err != GRF_NO_ERROR)
    {
        printf("grferr: %d\n", err);
        ret = -1;
        goto exit;
    }

    // Ensure that there's a tileset or bitmap

    if (gfxData == NULL)
    {
        printf("No graphics data\n");
        ret = -2;
        goto exit;
    }

    // Check palette

    int bpp = 0;

    if ((header.gfxAttr == 4) || (header.gfxAttr == 8) || (header.gfxAttr == 16))
    {
        bpp = header.gfxAttr;
    }
    else
    {
        printf("Unsupported bpp %u\n", header.gfxAttr);
        ret = -3;
        goto exit;
    }

    if ((bpp == 4) || (bpp == 8))
    {
        if (palData == NULL)
        {
            printf("No palette data\n");
            ret = -4;
            goto exit;
        }
    }

    // Determine the size of the background

    BgSize size = -1;

    if (mapData)
    {
        if (header.mapAttr == GRF_BGFMT_NO_DATA)
        {
            printf("No map layout format defined\n");
            ret = -5;
            goto exit;
        }

        if (header.mapAttr > GRF_BGFMT_FLAT_4BPP)
        {
            printf("Unknown map layout: %u\n", header.mapAttr);
            ret = -6;
            goto exit;
        }

        const char *names[] = {
            [GRF_BGFMT_SBB_4BPP] = "GRF_BGFMT_SBB_4BPP",
            [GRF_BGFMT_SBB_8BPP] = "GRF_BGFMT_SBB_8BPP",
            [GRF_BGFMT_AFF_8BPP] = "GRF_BGFMT_AFF_8BPP",
            [GRF_BGFMT_FLAT_8BPP] = "GRF_BGFMT_FLAT_8BPP",
            [GRF_BGFMT_FLAT_4BPP] = "GRF_BGFMT_FLAT_4BPP",
        };

        printf("%s\n", names[header.mapAttr]);

        switch (header.mapAttr)
        {
            case GRF_BGFMT_FLAT_8BPP:
                if (type == BgType_ExRotation)
                {
                    if (header.gfxWidth == 128 && header.gfxHeight == 128)
                        size = BgSize_ER_128x128;
                    else if (header.gfxWidth == 256 && header.gfxHeight == 256)
                        size = BgSize_ER_256x256;
                    else if (header.gfxWidth == 512 && header.gfxHeight == 512)
                        size = BgSize_ER_512x512;
                    else if (header.gfxWidth == 1024 && header.gfxHeight == 1024)
                        size = BgSize_ER_1024x1024;
                }
                // Fallthrough
            case GRF_BGFMT_SBB_8BPP:
                if (type == BgType_Text8bpp)
                {
                    if (header.gfxWidth == 256 && header.gfxHeight == 256)
                        size = BgSize_T_256x256;
                    else if (header.gfxWidth == 512 && header.gfxHeight == 256)
                        size = BgSize_T_512x256;
                    else if (header.gfxWidth == 256 && header.gfxHeight == 512)
                        size = BgSize_T_256x512;
                    else if (header.gfxWidth == 512 && header.gfxHeight == 512)
                        size = BgSize_T_512x512;
                }
                break;

            case GRF_BGFMT_FLAT_4BPP:
            case GRF_BGFMT_SBB_4BPP:
                if (type == BgType_Text4bpp)
                {
                    if (header.gfxWidth == 256 && header.gfxHeight == 256)
                        size = BgSize_T_256x256;
                    else if (header.gfxWidth == 512 && header.gfxHeight == 256)
                        size = BgSize_T_512x256;
                    else if (header.gfxWidth == 256 && header.gfxHeight == 512)
                        size = BgSize_T_256x512;
                    else if (header.gfxWidth == 512 && header.gfxHeight == 512)
                        size = BgSize_T_512x512;
                }
                break;

            case GRF_BGFMT_AFF_8BPP:
                if (type == BgType_Rotation)
                {
                    if (header.gfxWidth == 128 && header.gfxHeight == 128)
                        size = BgSize_R_128x128;
                    else if (header.gfxWidth == 256 && header.gfxHeight == 256)
                        size = BgSize_R_256x256;
                    else if (header.gfxWidth == 512 && header.gfxHeight == 512)
                        size = BgSize_R_512x512;
                    else if (header.gfxWidth == 1024 && header.gfxHeight == 1024)
                        size = BgSize_R_1024x1024;
                }
                break;

            default:
                break;
        }
    }
    else
    {
        if (bpp == 8)
        {
            printf("8 BPP BITMAP\n");

            if (type == BgType_Bmp8)
            {
                if (header.gfxWidth == 128 && header.gfxHeight == 128)
                    size = BgSize_B8_128x128;
                else if (header.gfxWidth == 256 && header.gfxHeight == 256)
                    size = BgSize_B8_256x256;
                else if (header.gfxWidth == 512 && header.gfxHeight == 256)
                    size = BgSize_B8_512x256;
                else if (header.gfxWidth == 512 && header.gfxHeight == 512)
                    size = BgSize_B8_512x512;
                else if (header.gfxWidth == 1024 && header.gfxHeight == 512)
                    size = BgSize_B8_1024x512;
                else if (header.gfxWidth == 512 && header.gfxHeight == 1024)
                    size = BgSize_B8_512x1024;
            }
        }
        else if (bpp == 16)
        {
            printf("16 BPP BITMAP\n");

            if (type == BgType_Bmp16)
            {
                if (header.gfxWidth == 128 && header.gfxHeight == 128)
                    size = BgSize_B16_128x128;
                else if (header.gfxWidth == 256 && header.gfxHeight == 256)
                    size = BgSize_B16_256x256;
                else if (header.gfxWidth == 512 && header.gfxHeight == 256)
                    size = BgSize_B16_512x256;
                else if (header.gfxWidth == 512 && header.gfxHeight == 512)
                    size = BgSize_B16_512x512;
            }
        }
    }

    if (size == -1)
    {
        // This example only supports the hardware-supported sizes.
        printf("Invalid type and size: %d, %ux%u\n", type,
               (unsigned int)header.gfxWidth, (unsigned int)header.gfxHeight);
        ret = -7;
        goto exit;
    }

    // Setup background layer

    int bg = bgInit(layer, type, size, map_base, tile_base);

    // Copy background graphics

    memcpy(bgGetGfxPtr(bg), gfxData, gfxSize);

    if (palData)
        memcpy(BG_PALETTE, palData, palSize);

    if (mapData)
        memcpy(bgGetMapPtr(bg), mapData, mapSize);

    // Cleanup

    ret = bg;
exit:
    free(mapData);
    free(gfxData);
    free(palData);

    return ret;
}

int main(int argc, char *argv[])
{
    // Video mode 4 configures the background layers like this:
    //
    // - Layer 0: Regular background.
    // - Layer 1: Regular background.
    // - Layer 2: Affine background.
    // - Layer 3: Extended background (rotation / bitmap).
    //
    videoSetMode(MODE_4_2D);
    videoSetModeSub(MODE_0_2D);

    // VRAM_C is used by consoleDemoInit()
    vramSetPrimaryBanks(VRAM_A_MAIN_BG_0x06000000,
                        VRAM_B_MAIN_BG_0x06020000,
                        VRAM_C_LCD, VRAM_D_LCD);

    consoleDemoInit();

    printf("Initializing NitroFS...\n");

    if (!nitroFSInit(NULL))
    {
        perror("nitroFSInit()");
        wait_forever();
    }

    typedef struct
    {
        const char *title;
        const char *path;
        int bg_layer;
        BgType type;
        int map_base, tile_base;
    }
    map_info_t;

    map_info_t map_info[] =
    {
        {
            .title = "Regular map: 4 bpp",
            .path = "grit/bg_regular_4bpp_png.grf",
            .bg_layer = 0,
            .type = BgType_Text4bpp,
            .map_base = 0,
            .tile_base = 1
        },
        {
            .title = "Regular map: 8 bpp",
            .path = "grit/bg_regular_8bpp_png.grf",
            .bg_layer = 0,
            .type = BgType_Text8bpp,
            .map_base = 0,
            .tile_base = 1
        },
        {
            .title = "Affine map",
            .path = "grit/bg_rotation_png.grf",
            .bg_layer = 2,
            .type = BgType_Rotation,
            .map_base = 0,
            .tile_base = 1
        },
        {
            .title = "Extended affine map",
            .path = "grit/bg_ext_rotation_png.grf",
            .bg_layer = 3,
            .type =  BgType_ExRotation,
            .map_base = 0,
            .tile_base = 1
        },
        {
            .title = "Bitmap: 8 bpp",
            .path = "grit/bmp_8bit_png.grf",
            .bg_layer = 3,
            .type = BgType_Bmp8,
            .map_base = 0,
            .tile_base = 0
        },
        {
            .title = "Bitmap: 16 bpp",
            .path = "grit/bmp_16bit_png.grf",
            .bg_layer = 3,
            .type = BgType_Bmp16,
            .map_base = 0,
            .tile_base = 0
        },
        {
            .title = NULL,
            .path = NULL,
            .bg_layer = 0,
            .type = 0,
            .map_base = 0,
            .tile_base = 0
        }
    };

    for (int i = 0; ; i++)
    {
        map_info_t * info = &map_info[i];

        if (info->title == NULL)
            break;

        consoleClear();
        printf("%s\n", info->title);
        printf("\n");
        backgroundLoadGrf(info->path, info->bg_layer, info->type,
                          info->map_base, info->tile_base);
        printf("\n");
        printf("A: Next scene\n");

        wait_press_a();

        bgHide(info->bg_layer);
    }

    consoleClear();
    printf("START: Exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
