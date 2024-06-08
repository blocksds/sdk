// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <nds.h>

__attribute__((noreturn)) void wait_forever(void)
{
    printf("Press START to exit.");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysHeld() & KEY_START)
            exit(1);
    }
}

// -----------------------------------------------------------------------------

#define NUM_TEXTURES 20

int textureID[NUM_TEXTURES];
static uint32_t tex_buffer[512 * 1024];

size_t texture_size(size_t dimension, GL_TEXTURE_TYPE_ENUM format)
{
    unsigned int bpp[9] = {
        0, 8, 2, 4, 8, 3, 8, 16, 16
    };

    return (dimension * 512 * bpp[format]) / 8;
}

int allocate_texture(int texture_index, int fill_value, size_t dimension,
                     GL_TEXTURE_TYPE_ENUM format)
{
    memset(tex_buffer, fill_value, texture_size(dimension, format));

    glBindTexture(0, textureID[texture_index]);

    if (glTexImage2D(0, 0, format, dimension, 512, 0, TEXGEN_TEXCOORD,
                     tex_buffer) == 0)
        return -1;

    if ((format == GL_RGBA) || (format == GL_RGB))
        return 0;

    int num_colors = 0;
    if (format == GL_RGB4)
        num_colors = 4;
    else if (format == GL_RGB16)
        num_colors = 16;
    else if (format == GL_RGB256)
        num_colors = 256;
    else if (format == GL_RGB32_A3)
        num_colors = 32;
    else if (format == GL_RGB8_A5)
        num_colors = 8;
    else if (format == GL_COMPRESSED)
        num_colors = 128; // Arbitrary value, this isn't a fixed value

    if (glColorTableEXT(0, 0, num_colors, 0, 0, tex_buffer) == 0)
    {
        glTexImage2D(0, 0, GL_NOTEXTURE, 0, 0, 0, 0, NULL);
        return -2;
    }

    return 0;
}

int free_texture(int texture_index, GL_TEXTURE_TYPE_ENUM format)
{
    glBindTexture(0, textureID[texture_index]);

    if (glTexImage2D(0, 0, GL_NOTEXTURE, 0, 0, 0, 0, NULL) == 0)
        return -1;

    if ((format == GL_RGBA) || (format == GL_RGB))
        return 0;

    if (glColorTableEXT(0, 0, 0, 0, 0, NULL) == 0)
        return -2;

    return 0;
}

// -----------------------------------------------------------------------------

void do_teximage2d_tests(void)
{
    // Note that "0" is a valid size. It's equivalent to TEXTURE_SIZE_8.

    printf("glTexImage2D() tests\n");
    printf("====================\n");
    printf("\n");

    glBindTexture(0, textureID[0]);

    // Try to allocate with no available VRAM

    vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);

    if (glTexImage2D(0, 0, GL_RGBA, 128, 128, 0, TEXGEN_TEXCOORD, tex_buffer) == 1)
    {
        printf("%d: Allocation should have failed\n", __LINE__);
        wait_forever();
    }

    vramSetPrimaryBanks(VRAM_A_TEXTURE, VRAM_B_TEXTURE, VRAM_C_TEXTURE, VRAM_D_TEXTURE);

    // Try to allocate with sizes that are negative

    if (glTexImage2D(0, 0, GL_RGBA, -128, 128, 0, TEXGEN_TEXCOORD, tex_buffer) == 1)
    {
        printf("%d: Allocation should have failed\n", __LINE__);
        wait_forever();
    }

    if (glTexImage2D(0, 0, GL_RGBA, 128, -128, 0, TEXGEN_TEXCOORD, tex_buffer) == 1)
    {
        printf("%d: Allocation should have failed\n", __LINE__);
        wait_forever();
    }

    if (glTexImage2D(0, 0, GL_RGBA, -128, -128, 0, TEXGEN_TEXCOORD, tex_buffer) == 1)
    {
        printf("%d: Allocation should have failed\n", __LINE__);
        wait_forever();
    }

    // Try to allocate a texture with valid sizes, then deallocate it

    if (glTexImage2D(0, 0, GL_RGBA, 128, 128, 0, TEXGEN_TEXCOORD, tex_buffer) == 0)
    {
        printf("%d: Allocation should have succeeded\n", __LINE__);
        wait_forever();
    }
    if (glTexImage2D(0, 0, GL_NOTEXTURE, 0, 0, 0, TEXGEN_TEXCOORD, NULL) == 0)
    {
        printf("%d: Deallocation should have succeeded\n", __LINE__);
        wait_forever();
    }

    // Try to allocate with sizes that aren't power of two

    if (glTexImage2D(0, 0, GL_RGBA, 57, 128, 0, TEXGEN_TEXCOORD, tex_buffer) == 1)
    {
        printf("%d: Allocation should have failed\n", __LINE__);
        wait_forever();
    }

    if (glTexImage2D(0, 0, GL_RGBA, 128, 57, 0, TEXGEN_TEXCOORD, tex_buffer) == 1)
    {
        printf("%d: Allocation should have failed\n", __LINE__);
        wait_forever();
    }

    // Try to allocate textures with sizes that are too big

    if (glTexImage2D(0, 0, GL_RGBA, 2048, 128, 0, TEXGEN_TEXCOORD, tex_buffer) == 1)
    {
        printf("%d: Allocation should have failed\n", __LINE__);
        wait_forever();
    }

    if (glTexImage2D(0, 0, GL_RGBA, 128, 2048, 0, TEXGEN_TEXCOORD, tex_buffer) == 1)
    {
        printf("%d: Allocation should have failed\n", __LINE__);
        wait_forever();
    }

    // Try to allocate with an invalid format

    if (glTexImage2D(0, 0, 51, 128, 128, 0, TEXGEN_TEXCOORD, tex_buffer) == 1)
    {
        printf("%d: Allocation should have failed\n", __LINE__);
        wait_forever();
    }

    // Try to allocate with a NULL pointer (allocate texture but don't copy
    // data to it).

    if (glTexImage2D(0, 0, GL_RGBA, 128, 128, 0, TEXGEN_TEXCOORD, NULL) == 0)
    {
        printf("%d: Allocation should have succeeded\n", __LINE__);
        wait_forever();
    }
    if (glTexImage2D(0, 0, GL_NOTEXTURE, 0, 0, 0, TEXGEN_TEXCOORD, NULL) == 0)
    {
        printf("%d: Deallocation should have succeeded\n", __LINE__);
        wait_forever();
    }

    // Try to allocate with no active texture

    glBindTexture(0, 0);

    if (glTexImage2D(0, 0, GL_RGBA, 128, 128, 0, TEXGEN_TEXCOORD, tex_buffer) == 1)
    {
        printf("%d: Allocation should have failed\n", __LINE__);
        wait_forever();
    }

    printf("SUCCESS\n");
    printf("\n");
}

void do_colortableext_tests(void)
{
    printf("glColorTableEXT() tests\n");
    printf("=======================\n");
    printf("\n");

    glBindTexture(0, textureID[0]);

    // Try to allocate with no available VRAM

    vramSetBankE(VRAM_E_LCD);
    vramSetBankF(VRAM_F_LCD);

    if (glColorTableEXT(0, 0, 64, 0, 0, tex_buffer) == 1)
    {
        printf("%d: Allocation should have failed\n", __LINE__);
        wait_forever();
    }

    vramSetBankE(VRAM_E_TEX_PALETTE);
    vramSetBankF(VRAM_F_TEX_PALETTE);

    // Try to allocate with a negative size

    if (glColorTableEXT(0, 0, -64, 0, 0, tex_buffer) == 1)
    {
        printf("%d: Allocation should have failed\n", __LINE__);
        wait_forever();
    }

    // Try to allocate a texture with a valid size, then deallocate it

    if (glColorTableEXT(0, 0, 64, 0, 0, tex_buffer) == 0)
    {
        printf("%d: Allocation should have succeeded\n", __LINE__);
        wait_forever();
    }
    if (glColorTableEXT(0, 0, 0, 0, 0, NULL) == 0)
    {
        printf("%d: Deallocation should have succeeded\n", __LINE__);
        wait_forever();
    }

    // Try to allocate with no active texture

    glBindTexture(0, 0);

    if (glColorTableEXT(0, 0, 64, 0, 0, tex_buffer) == 1)
    {
        printf("%d: Allocation should have failed\n", __LINE__);
        wait_forever();
    }

    printf("SUCCESS\n");
    printf("\n");
}

// -----------------------------------------------------------------------------

#define ALLOC_EXPECT(index, dim, expect)                                    \
    do                                                                      \
    {                                                                       \
        int ret = allocate_texture(index, index * 13, dim, GL_COMPRESSED);  \
        if (ret != expect)                                                  \
        {                                                                   \
            printf("%d: Unexpected value: %d != %d\n", __LINE__,            \
                   ret, expect);                                            \
            wait_forever();                                                 \
        }                                                                   \
    } while (0);

#define FREE_EXPECT(index, expect)                                          \
    do                                                                      \
    {                                                                       \
        int ret = free_texture(index, GL_COMPRESSED);                       \
        if (ret != expect)                                                  \
        {                                                                   \
            printf("%d: Unexpected value: %d != %d\n", __LINE__,            \
                   ret, expect);                                            \
            wait_forever();                                                 \
        }                                                                   \
    } while (0);

void do_compressed_texture_tests(void)
{
    printf("GL_COMPRESSED allocation tests\n");
    printf("==============================\n");
    printf("\n");

    // [*] Try to allocate when there are no available texture or palette banks

    vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);
    vramSetBankE(VRAM_E_LCD);
    vramSetBankF(VRAM_F_LCD);
    ALLOC_EXPECT(0, 128, -1);

    // [*] Try to allocate when there are no available texture banks

    vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);

    vramSetBankE(VRAM_E_TEX_PALETTE);
    vramSetBankF(VRAM_F_LCD);
    ALLOC_EXPECT(0, 128, -1);

    vramSetBankE(VRAM_E_LCD);
    vramSetBankF(VRAM_F_TEX_PALETTE);
    ALLOC_EXPECT(0, 128, -1);

    // [*] Try to allocate when there are no available palette banks

    vramSetBankE(VRAM_E_LCD);
    vramSetBankF(VRAM_F_LCD);

    vramSetPrimaryBanks(VRAM_A_TEXTURE, VRAM_B_TEXTURE, VRAM_C_TEXTURE, VRAM_D_TEXTURE);
    ALLOC_EXPECT(0, 128, -2);

    vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_TEXTURE, VRAM_C_TEXTURE, VRAM_D_LCD);
    ALLOC_EXPECT(0, 128, -2);

    vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_TEXTURE, VRAM_D_LCD);
    ALLOC_EXPECT(0, 128, -1);

    vramSetPrimaryBanks(VRAM_A_TEXTURE, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);
    ALLOC_EXPECT(0, 128, -1);

    // [*] Try to allocate when VRAM_B is not setup as texture VRAM

    vramSetPrimaryBanks(VRAM_A_TEXTURE, VRAM_B_LCD, VRAM_C_TEXTURE, VRAM_D_TEXTURE);
    vramSetBankE(VRAM_E_TEX_PALETTE);
    vramSetBankF(VRAM_F_TEX_PALETTE);
    ALLOC_EXPECT(0, 128, -1);

    // [*] Try to allocate when VRAM_A and VRAM_C are not setup as texture VRAM

    vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_TEXTURE, VRAM_C_LCD, VRAM_D_TEXTURE);
    vramSetBankE(VRAM_E_TEX_PALETTE);
    vramSetBankF(VRAM_F_TEX_PALETTE);
    ALLOC_EXPECT(0, 128, -1);

    // [*] Try to allocate normally with VRAM A, B and C

    vramSetPrimaryBanks(VRAM_A_TEXTURE, VRAM_B_TEXTURE, VRAM_C_TEXTURE, VRAM_D_LCD);
    vramSetBankE(VRAM_E_TEX_PALETTE);
    vramSetBankF(VRAM_F_TEX_PALETTE);

    ALLOC_EXPECT(0, 512, 0);
    ALLOC_EXPECT(1, 512, 0);
    ALLOC_EXPECT(2, 512, 0);
    ALLOC_EXPECT(3, 512, 0);
    ALLOC_EXPECT(4, 512, -1); // VRAM is full!
    FREE_EXPECT(0, 0);
    FREE_EXPECT(1, 0);
    FREE_EXPECT(2, 0);
    FREE_EXPECT(3, 0);

    // [*] Try to allocate normally with VRAM A and B only

    vramSetPrimaryBanks(VRAM_A_TEXTURE, VRAM_B_TEXTURE, VRAM_C_LCD, VRAM_D_LCD);
    vramSetBankE(VRAM_E_TEX_PALETTE);
    vramSetBankF(VRAM_F_TEX_PALETTE);

    ALLOC_EXPECT(0, 512, 0);
    ALLOC_EXPECT(1, 1024, -1); // Not enough memory!
    ALLOC_EXPECT(1, 512, 0);
    ALLOC_EXPECT(2, 512, -1); // VRAM is full!
    FREE_EXPECT(0, 0);
    FREE_EXPECT(1, 0);

    // [*] Try to allocate normally with VRAM B and C only

    vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_TEXTURE, VRAM_C_TEXTURE, VRAM_D_LCD);
    vramSetBankE(VRAM_E_TEX_PALETTE);
    vramSetBankF(VRAM_F_TEX_PALETTE);

    ALLOC_EXPECT(0, 512, 0);
    ALLOC_EXPECT(1, 1024, -1); // Not enough memory!
    ALLOC_EXPECT(1, 512, 0);
    ALLOC_EXPECT(2, 512, -1); // VRAM is full!
    FREE_EXPECT(0, 0);
    FREE_EXPECT(1, 0);

    // [*] Try to allocate a texture that doesn't fit fully in either VRAM_A or
    // C but would fit if it was split in both banks. This isn't allowed.

    vramSetPrimaryBanks(VRAM_A_TEXTURE, VRAM_B_TEXTURE, VRAM_C_TEXTURE, VRAM_D_LCD);
    vramSetBankE(VRAM_E_TEX_PALETTE);
    vramSetBankF(VRAM_F_TEX_PALETTE);

    ALLOC_EXPECT(0, 512, 0);
    ALLOC_EXPECT(1, 512, 0);
    ALLOC_EXPECT(2, 512, 0);
    ALLOC_EXPECT(3, 512, 0);

    //  VRAM A  |  VRAM B  |  VRAM C
    // ---------+----------+----------
    // 00001111 | 00112233 | 22223333

    FREE_EXPECT(1, 0); // Free the two textures in the middle
    FREE_EXPECT(2, 0);

    //  VRAM A  |  VRAM B  |  VRAM C
    // ---------+----------+----------
    // 0000.... | 00....33 | ....3333

    ALLOC_EXPECT(4, 1024, -1); // Try to allocate a big texture, but fail

    ALLOC_EXPECT(1, 512, 0); // Allocating two small textures works
    ALLOC_EXPECT(2, 512, 0);

    FREE_EXPECT(0, 0);
    FREE_EXPECT(1, 0);
    FREE_EXPECT(2, 0);
    FREE_EXPECT(3, 0);

    printf("SUCCESS\n");
    printf("\n");
}

// -----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    memset(tex_buffer, 0, sizeof(tex_buffer));

    // Setup sub screen for the text console. consoleDemoInit() can't be used as
    // we are going to use VRAM_A to VRAM_F for the tests.
    videoSetModeSub(MODE_0_2D);
    vramSetBankH(VRAM_H_SUB_BG);
    consoleInit(NULL, 1, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);

    videoSetMode(MODE_0_3D);

    glInit();

    // Generate IDs for textures
    glGenTextures(NUM_TEXTURES, &textureID[0]);

    do_teximage2d_tests();
    do_colortableext_tests();

    do_compressed_texture_tests();

    glDeleteTextures(NUM_TEXTURES, &textureID[0]);

    wait_forever();

    return 0;
}
