// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <gl2d.h>
#include <filesystem.h>
#include <nds.h>

const uint32_t screen_width = 256;
const uint32_t screen_height = 192;

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

void *load_file(const char *filename, size_t *size_)
{
    FILE *f = fopen(filename, "rb");
    if (f == NULL)
    {
        printf("fopen(): %s", filename);
        return NULL;
    }

    if (fseek(f, 0, SEEK_END) != 0)
    {
        printf("fseek(): %s", filename);
        fclose(f);
        return NULL;
    }

    size_t size = ftell(f);
    rewind(f);

    if (size_)
        *size_ = size;

    char *buffer = malloc(size);
    if (buffer == NULL)
    {
        printf("malloc(): %s", filename);
        fclose(f);
        return NULL;
    }

    if (fread(buffer, 1, size, f) != size)
    {
        printf("fread(): %s", filename);
        free(buffer);
        fclose(f);
        return NULL;
    }

    if (fclose(f) != 0)
    {
        printf("fclose(): %s", filename);
        free(buffer);
        return NULL;
    }

    return buffer;
}

glImage *gl2d_load_nitrofs_sprite_set(int *texture_id, int *texture_frames,
                                      const char *guv_path, const char *grf_path)
{
    uint16_t *texture_guv = load_file(guv_path, NULL);
    if (texture_guv == NULL)
        wait_forever();

    if (texture_guv[0] != 0x012D)
    {
        printf("Invalid magic: %s\n", guv_path);
        wait_forever();
    }

    uint16_t texture_width = texture_guv[1];
    uint16_t texture_height = texture_guv[2];
    *texture_frames = texture_guv[3];

    void *gfxDst = NULL;
    void *palDst = NULL;
    GRFHeader header = { 0 };
    GRFError err = grfLoadPath(grf_path, &header,
                               &gfxDst, NULL, NULL, NULL, &palDst, NULL);
    if (err != GRF_NO_ERROR)
    {
        printf("Couldn't load GRF file: %d", err);
        wait_forever();
    }

    if (header.gfxWidth != texture_width)
    {
        printf("Width of GUV and GRF don't match");
        wait_forever();
    }

    if (header.gfxHeight != texture_height)
    {
        printf("Height of GUV and GRF don't match");
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

    // Expect the texture to be a 256 color texture
    if (header.gfxAttr != 8)
    {
        printf("Invalid format in GRF file");
        wait_forever();
    }

    glImage *images = calloc(sizeof(glImage), *texture_frames);
    if (images == NULL)
    {
        printf("Can't allocate space for frames");
        wait_forever();
    }

    *texture_id =
        glLoadSpriteSet(images,             // Pointer to glImage array
                        *texture_frames,    // Number of images
                        texture_guv + 4,    // Array of UV coordinates
                        GL_RGB256,      // Texture type for glTexImage2D()
                        texture_width,  // Full texture size X (image size)
                        texture_height, // Full texture size Y (image size)
                        // Parameters for glTexImage2D()
                        TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT,
                        header.palAttr, // Length of the palette to use
                        palDst,  // Pointer to texture palette data
                        gfxDst); // Pointer to texture data

    if (*texture_id < 0)
    {
        printf("Failed to load texture: %d\n", *texture_id);
        wait_forever();
    }

    free(gfxDst);
    free(palDst);
    free(texture_guv);

    return images;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("Initializing NitroFS...\n");

    bool init_ok = nitroFSInit(NULL);
    if (!init_ok)
    {
        perror("nitroFSInit()");
        wait_forever();
    }

    // Initialize OpenGL to some sensible defaults
    glScreen2D();

    videoSetMode(MODE_0_3D);

    // Setup some memory to be used for textures and for texture palettes
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankE(VRAM_E_TEX_PALETTE);

    // A sprite set is one big texture formed by several images of different
    // sizes. The location and size of each image is stored in the GUV file. The
    // actual texture (and its palette, if required) is stored in the GRF file.
    //
    // The GPU of the DS requires textures to have sizes that are power of
    // two. When you have a bitmap with dimensions that aren't a power of
    // two, padding needs to be added to the bottom and to the right to fill
    // the image up to a valid size.
    //
    // Combining multiple non-power-of-two images into a single power-of-two
    // texture allows you to use VRAM more efficiently.
    int ruins_texture_id;
    int texture_frames;
    glImage *ruins = gl2d_load_nitrofs_sprite_set(&ruins_texture_id,
                                &texture_frames,
                                "nitro:/gl2d/ruins/ruins.guv",
                                "nitro:/gl2d/ruins/ruins_texture.grf");

    // Print some controls
    printf("START:  Exit to loader\n");
    printf("\n");

    int delay = 0;
    int frame = 0;

    while (1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        // Handle user input
        // -----------------

        scanKeys();

        uint16_t keys = keysHeld();
        if (keys & KEY_START)
            break;

        // Render 3D scene
        // ---------------

        // Set up GL2D for 2D mode
        glBegin2D();

            // Fill screen with a gradient
            glBoxFilledGradient(0, 0,
                                screen_width - 1, screen_height - 1,
                                RGB15(31, 0, 0),
                                RGB15(31, 31, 0),
                                RGB15(31, 0, 31),
                                RGB15(0, 31, 31));

            // Draw sprite frames individually
            glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(0));
            glColor(RGB15(31, 31, 31));

            glSprite(0, 4, GL_FLIP_NONE, &ruins[0]);
            glSprite(64, 4, GL_FLIP_NONE, &ruins[1]);
            glSprite(128, 4, GL_FLIP_NONE, &ruins[2]);
            glSprite(192, 4, GL_FLIP_NONE, &ruins[3]);
            glSprite(0, 100, GL_FLIP_NONE, &ruins[4]);
            glSprite(80, 100, GL_FLIP_NONE, &ruins[5]);
            glSprite(180, 100, GL_FLIP_NONE, &ruins[6]);

            // Draw animated sprite
            glSprite(80, 60, GL_FLIP_NONE, &ruins[frame]);

            // Animate (change animation frame every 10 frames)
            delay++;
            if (delay == 10)
            {
                delay = 0;

                frame++;
                if (frame == texture_frames)
                    frame = 0;
            }

        glEnd2D();

        glFlush(0);
    }

    free(ruins);
    glDeleteTextures(1, &ruins_texture_id);

    return 0;
}
