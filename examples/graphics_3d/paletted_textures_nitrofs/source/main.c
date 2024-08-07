// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// This example shows how to load paletted textures. It also shows how to set
// color 0 as transparent or not (for 4, 16 and 256 color textures).

#include <stdio.h>

#include <filesystem.h>
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

int main(int argc, char **argv)
{
    int textureID[2];

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

    // The background must be fully opaque and have a unique polygon ID
    // (different from the polygons that are going to be drawn) so that
    // antialias works.
    glClearColor(10, 10, 10, 31);
    glClearPolyID(63);

    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);

    // Setup some VRAM as memory for textures and texture palettes
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankF(VRAM_F_TEX_PALETTE);

    // Generate IDs for two textures
    glGenTextures(2, &textureID[0]);

    {
        void *gfxDst = NULL;
        void *palDst = NULL;
        GRFHeader header = { 0 };
        GRFError err = grfLoadPath("grit/ball_png.grf", &header, &gfxDst, NULL,
                                   NULL, NULL, &palDst, NULL);
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

        // Load texture once while setting color 0 as transparent
        glBindTexture(0, textureID[0]);
        if (glTexImage2D(0, 0, GL_RGB256, header.gfxWidth, header.gfxHeight, 0,
                         TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT, gfxDst) == 0)
        {
            printf("Failed to load texture 1\n");
            wait_forever();
        }
        if (glColorTableEXT(0, 0, header.palAttr, 0, 0, palDst) == 0)
        {
            printf("Failed to load palette 1\n");
            wait_forever();
        }

        // Load the same texture but this time don't set color 0 as transparent
        glBindTexture(0, textureID[1]);
        if (glTexImage2D(0, 0, GL_RGB256, header.gfxWidth, header.gfxHeight, 0,
                         TEXGEN_TEXCOORD, gfxDst) == 0)
        {
            printf("Failed to load texture 2\n");
            wait_forever();
        }
        if (glColorTableEXT(0, 0, header.palAttr, 0, 0, palDst) == 0)
        {
            printf("Failed to load palette 2\n");
            wait_forever();
        }
    }

    // Setup matrices
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.1, 40);

    gluLookAt(0.0, 0.0, 3.0,  // Position
              0.0, 0.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up

    while (1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        // Print some text in the demo console
        // -----------------------------------

        consoleClear();

        // Print some controls
        printf("START:  Exit to loader\n");
        printf("\n");

        // Handle user input
        // -----------------

        scanKeys();

        uint16_t keys = keysHeld();
        if (keys & KEY_START)
            break;

        // Render 3D scene
        // ---------------

        glMatrixMode(GL_MODELVIEW);

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

        glColor3f(1, 1, 1);

        glBindTexture(0, textureID[0]);

        glPushMatrix();

            glTranslatef(-1.5, 0, 0);

            glBegin(GL_QUADS);

                GFX_TEX_COORD = (TEXTURE_PACK(0, inttot16(32)));
                glVertex3v16(floattov16(-1), floattov16(-1), floattov16(-0.2));

                GFX_TEX_COORD = (TEXTURE_PACK(inttot16(32),inttot16(32)));
                glVertex3v16(floattov16(1), floattov16(-1), floattov16(-0.2));

                GFX_TEX_COORD = (TEXTURE_PACK(inttot16(32), 0));
                glVertex3v16(floattov16(1), floattov16(1), floattov16(-0.2));

                GFX_TEX_COORD = (TEXTURE_PACK(0,0));
                glVertex3v16(floattov16(-1), floattov16(1), floattov16(-0.2));

            glEnd();

        glPopMatrix(1);

        glPushMatrix();

            glTranslatef(1.5, 0, 0);

            glBindTexture(0, textureID[1]);

            glBegin(GL_QUADS);

                GFX_TEX_COORD = (TEXTURE_PACK(0, inttot16(32)));
                glVertex3v16(floattov16(-1), floattov16(-1), 0);

                GFX_TEX_COORD = (TEXTURE_PACK(inttot16(32),inttot16(32)));
                glVertex3v16(floattov16(1), floattov16(-1), 0);

                GFX_TEX_COORD = (TEXTURE_PACK(inttot16(32), 0));
                glVertex3v16(floattov16(1), floattov16(1), 0);

                GFX_TEX_COORD = (TEXTURE_PACK(0,0));
                glVertex3v16(floattov16(-1), floattov16(1), 0);

            glEnd();

        glPopMatrix(1);

        glFlush(0);
    }

    glDeleteTextures(2, &textureID[0]);

    return 0;
}
