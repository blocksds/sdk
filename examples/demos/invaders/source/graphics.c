// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <gl2d.h>
#include <nds.h>

#include "definitions.h"
#include "helpers.h"

int texture_id_ship;
int texture_id_bullet;

int invaders_texture_id;
int invaders_frames;
glImage *invader_sprites;

void load_grf_8_bit_texture(const char *path)
{
    void *gfxDst = NULL;
    void *palDst = NULL;
    size_t gfxSize, palSize;
    GRFHeader header = { 0 };
    GRFError err = grfLoadPath(path, &header,
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

    // Load texture while setting color 0 as transparent
    if (glTexImage2D(0, 0, GL_RGB256, header.gfxWidth, header.gfxHeight, 0,
                        TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT,
                        gfxDst) == 0)
    {
        printf("Failed to load texture 1\n");
        wait_forever();
    }
    if (glColorTableEXT(0, 0, header.palAttr, 0, 0, palDst) == 0)
    {
        printf("Failed to load palette 1\n");
        wait_forever();
    }
}

void load_background(const char *path)
{
    void *gfxDst = NULL;
    void *mapDst = NULL;
    void *palDst = NULL;
    size_t gfxSize, mapSize, palSize;
    GRFHeader header = { 0 };
    GRFError err = grfLoadPath(path, &header, &gfxDst, &gfxSize,
                               &mapDst, &mapSize, &palDst, &palSize);
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

    int bg = bgInit(3, BgType_Text8bpp, BgSize_T_256x256, 0, 1);

    // Flush cache so that we can use DMA to copy the data to VRAM
    DC_FlushAll();

    dmaCopy(gfxDst, bgGetGfxPtr(bg), gfxSize);
    dmaCopy(mapDst, bgGetMapPtr(bg), mapSize);
    dmaCopy(palDst, BG_PALETTE, palSize);
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
        glLoadSpriteSet(images,          // Pointer to glImage array
                        *texture_frames, // Number of images
                        texture_guv + 4, // Array of UV coordinates
                        GL_RGB256,       // Texture type for glTexImage2D()
                        texture_width,   // Full texture size X (image size)
                        texture_height,  // Full texture size Y (image size)
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

void load_sprites(void)
{
    // Generate texture ID, select it as an active texture, and load a GRF file
    // into the active texture.
    glGenTextures(1, &texture_id_ship);
    glBindTexture(0, texture_id_ship);
    load_grf_8_bit_texture("grit/sprites/ship_png.grf");

    glGenTextures(1, &texture_id_bullet);
    glBindTexture(0, texture_id_bullet);
    load_grf_8_bit_texture("grit/sprites/bullet_png.grf");

    // Load sprite set
    invader_sprites = gl2d_load_nitrofs_sprite_set(
                            &invaders_texture_id,
                            &invaders_frames,
                            "nitro:/gl2d/invaders/invaders.guv",
                            "nitro:/gl2d/invaders/invaders_texture.grf");

}

void free_graphics(void)
{
    glDeleteTextures(1, &texture_id_ship);
    glDeleteTextures(1, &texture_id_bullet);

    glDeleteTextures(1, &invaders_texture_id);
    free(invader_sprites);
}

void draw_invaders(invader *invaders)
{
    glBindTexture(0, invaders_texture_id);

    for (int i = 0; i < NUM_INVADERS; i++)
    {
        if (!invaders[i].active)
            continue;

        glSprite(invaders[i].x, invaders[i].y, GL_FLIP_NONE,
                 &invader_sprites[invaders[i].anim_frame]);
    }
}

void draw_ship(int x, int y)
{
    glBindTexture(0, texture_id_ship);

    int width = 0;
    int height = 0;
    glGetInt(GL_GET_TEXTURE_WIDTH, &width);
    glGetInt(GL_GET_TEXTURE_HEIGHT, &height);

    glPushMatrix();

        glTranslatef32(x, y, 0);

        glBegin(GL_QUADS);

            glTexCoord2i(0, 0);
            glVertex3v16(0, 0, 0);
            glTexCoord2i(0, height);
            glVertex2v16(0, height);
            glTexCoord2i(width, height);
            glVertex2v16(width, height);
            glTexCoord2i(width, 0);
            glVertex2v16(width, 0);

        glEnd();

    glPopMatrix(1);
}

void draw_bullets(bullet *bullets)
{
    glBindTexture(0, texture_id_bullet);

    int width = 0;
    int height = 0;
    glGetInt(GL_GET_TEXTURE_WIDTH, &width);
    glGetInt(GL_GET_TEXTURE_HEIGHT, &height);

    for (int i = 0; i < NUM_BULLETS; i++)
    {
        if (!bullets[i].active)
            continue;

        glPushMatrix();

            glTranslatef32(bullets[i].x, bullets[i].y, 0);

            glBegin(GL_QUADS);

                glTexCoord2i(0, 0);
                glVertex3v16(0, 0, 0);
                glTexCoord2i(0, height);
                glVertex2v16(0, height);
                glTexCoord2i(width, height);
                glVertex2v16(width, height);
                glTexCoord2i(width, 0);
                glVertex2v16(width, 0);

            glEnd();

        glPopMatrix(1);
    }
}
