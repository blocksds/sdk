// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025-2026

// This example shows how to load GRF files as textures. The code doesn't have
// any hardcoded sizes or formats, everything is read from the GRF file and the
// code loads the textures according to that information.
//
// This example compresses the textures using the appropriate grit compression
// arguments. Huffman compression isn't supported in some emulators. If you
// don't see some textures correctly, make sure you're using a dump of your BIOS
// in your emulator.

#include <stdio.h>

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

int load_texture_grf(const char *path, int *width, int *height)
{
    printf("%s\n", path);

    GRFHeader header = { 0 };
    void *gfxDst = NULL;
    void *pidxDst = NULL;
    void *palDst = NULL;
    size_t palSize = 0;

    GRFError err = grfLoadPath(path, &header, &gfxDst, NULL,
                               &pidxDst, NULL, &palDst, &palSize);
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

    int texture_format;
    int texture_params = 0;;

    if (header.gfxAttr == 16) // GL_RGBA
    {
        if (palDst)
        {
            printf("GL_RGBA texture with palette");
            wait_forever();
        }

        texture_format = GL_RGBA;
        printf("    GL_RGBA\n");
    }
    else if (header.gfxAttr == GRF_TEXFMT_4x4)
    {
        if (palDst == NULL)
        {
            printf("Tex4x4 texture without palette\n");
            wait_forever();
        }

        if (pidxDst == NULL)
        {
            printf("Tex4x4 texture without palette indices\n");
            wait_forever();
        }

        texture_format = GL_COMPRESSED;
        printf("    GL_COMPRESSED\n");
    }
    else
    {
        if (palDst == NULL)
        {
            printf("Paletted texture without palette");
            wait_forever();
        }

        if (pidxDst != NULL)
        {
            printf("Non-Tex4x4 texture with palette indices\n");
            wait_forever();
        }

        switch (header.gfxAttr)
        {
            case 8:
                texture_format = GL_RGB256;
                texture_params = GL_TEXTURE_COLOR0_TRANSPARENT;
                printf("    GL_RGB256\n");
                break;
            case 4:
                texture_format = GL_RGB16;
                texture_params = GL_TEXTURE_COLOR0_TRANSPARENT;
                printf("    GL_RGB16\n");
                break;
            case 2:
                texture_format = GL_RGB4;
                texture_params = GL_TEXTURE_COLOR0_TRANSPARENT;
                printf("    GL_RGB4\n");
                break;
            case GRF_TEXFMT_A5I3:
                texture_format = GL_RGB8_A5;
                printf("    GL_RGB8_A5\n");
                break;
            case GRF_TEXFMT_A3I5:
                texture_format = GL_RGB32_A3;
                printf("    GL_RGB32_A3\n");
                break;
            default:
                printf("Invalid format in GRF file");
                wait_forever();
        }
    }

    printf("    %lux%lu\n", header.gfxWidth, header.gfxHeight);

    // Load texture
    int textureID;
    if (glGenTextures(1, &textureID) == 0)
    {
        printf("Failed to generate texture\n");
        wait_forever();
    }

    glBindTexture(0, textureID);

    if (glTexImageNtr2D(texture_format, header.gfxWidth, header.gfxHeight,
                        TEXGEN_TEXCOORD | texture_params, gfxDst, pidxDst) == 0)
    {
        printf("Failed to load texture\n");
        wait_forever();
    }

    free(gfxDst);
    free(pidxDst);

    if (palDst)
    {
        if (glColorTableNtr(palSize / 2, palDst) == 0)
        {
            printf("Failed to load palette\n");
            wait_forever();
        }

        free(palDst);
    }

    if (width)
        *width = header.gfxWidth;

    if (height)
        *height = header.gfxHeight;

    return textureID;
}

int main(int argc, char **argv)
{
    // Setup sub screen for the text console
    consoleDemoInit();

    printf("Initializing NitroFS...\n");

    if (!nitroFSInit(NULL))
    {
        perror("nitroFSInit()");
        wait_forever();
    }

    videoSetMode(MODE_0_3D);

    glInit();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ANTIALIAS);
    glEnable(GL_BLEND);

    // The background must be fully opaque and have a unique polygon ID
    // (different from the polygons that are going to be drawn) so that
    // alpha blending works.
    glClearColor(0, 0, 0, 31);
    glClearPolyID(63);

    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);

    // Setup some VRAM as memory for textures
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    // VRAM_C is used for the demo console
    vramSetBankD(VRAM_D_TEXTURE);
    vramSetBankE(VRAM_E_TEX_PALETTE);

    typedef struct {
        int width, height;
        int id;
    } texture_info_t;

#define NUM_TEXTURES 4

    texture_info_t tex[NUM_TEXTURES] = { 0 };

    tex[0].id = load_texture_grf("tex/neon_png.grf", &tex[0].width, &tex[0].height);
    tex[1].id = load_texture_grf("tex/statue_png.grf", &tex[1].width, &tex[1].height);
    tex[2].id = load_texture_grf("tex/teapot_png.grf", &tex[2].width, &tex[2].height);
    tex[3].id = load_texture_grf("tex/compressed_png.grf", &tex[3].width, &tex[3].height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.1, 40);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 2.0,  // Position
              0.0, 0.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up

    int angle_x = 0;
    int angle_z = 0;

    // Print some text in the demo console
    // -----------------------------------

    // Print some controls
    printf("\n");
    printf("L/R:    Change texture\n");
    printf("PAD:    Rotate quad\n");
    printf("START:  Exit to loader\n");

    int index = 0;

    while (1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        // Handle user input
        // -----------------

        scanKeys();

        uint16_t keys_held = keysHeld();
        uint16_t keys_down = keysDown();

        if (keys_held & KEY_LEFT)
            angle_z += 3;
        if (keys_held & KEY_RIGHT)
            angle_z -= 3;

        if (keys_held & KEY_UP)
            angle_x += 3;
        if (keys_held & KEY_DOWN)
            angle_x -= 3;

        if (keys_held & KEY_START)
            break;

        if (keys_down & KEY_L)
        {
            index--;
            if (index == -1)
                index = NUM_TEXTURES - 1;
        }

        if (keys_down & KEY_R)
        {
            index++;
            if (index == NUM_TEXTURES)
                index = 0;
        }

        // Render 3D scene
        // ---------------

        // Draw a texture-less background polygon

        glBindTexture(0, 0); // Remove texture
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(0));

        glBegin(GL_QUADS);

            glColor3f(1, 0.5, 0.5);
            glVertex3v16(floattov16(-2), floattov16(-2), floattov16(-1));

            glColor3f(0.5, 1, 0.5);
            glVertex3v16(floattov16(2), floattov16(-2), floattov16(-1));

            glColor3f(0.5, 0.5, 1);
            glVertex3v16(floattov16(2), floattov16(2), floattov16(-1));

            glColor3f(1, 1, 0.5);
            glVertex3v16(floattov16(-2), floattov16(2), floattov16(-1));

        glEnd();

        // Draw a textured polygon

        glPushMatrix();

        glRotateZ(angle_z);
        glRotateX(angle_x);

        glBindTexture(0, tex[index].id);
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(1));

        glColor3f(1, 1, 1);

        glBegin(GL_QUADS);

            glTexCoord2t16(0, inttot16(tex[index].height));
            glVertex3v16(floattov16(-1), floattov16(-1), 0);

            glTexCoord2t16(inttot16(tex[index].width), inttot16(tex[index].height));
            glVertex3v16(floattov16(1), floattov16(-1), 0);

            glTexCoord2t16(inttot16(tex[index].width), 0);
            glVertex3v16(floattov16(1), floattov16(1), 0);

            glTexCoord2t16(0, 0);
            glVertex3v16(floattov16(-1), floattov16(1), 0);

        glEnd();

        glPopMatrix(1);

        glFlush(0);
    }

    glDeleteTextures(1, &tex[0].id);
    glDeleteTextures(1, &tex[1].id);
    glDeleteTextures(1, &tex[2].id);
    glDeleteTextures(1, &tex[3].id);

    return 0;
}
