// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

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

int main(int argc, char **argv)
{
    int textureID;

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
    glClearColor(0, 0, 0, 31);
    glClearPolyID(63);

    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);

    // Setup some VRAM as memory for textures
    vramSetBankA(VRAM_A_TEXTURE);

    void *gfxDst = NULL;
    GRFHeader header = { 0 };
    GRFError err = grfLoadPath("grit/neon_png.grf", &header, &gfxDst, NULL,
                               NULL, NULL, NULL, NULL);
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

    if (header.gfxAttr != 16)
    {
        printf("Invalid format in GRF file");
        wait_forever();
    }

    if ((header.gfxWidth != 128) || (header.gfxHeight != 128))
    {
        printf("Texture size needs to be 128x128");
        wait_forever();
    }

    // Load texture
    glGenTextures(1, &textureID);
    glBindTexture(0, textureID);
    glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_128 , TEXTURE_SIZE_128, 0,
                 TEXGEN_TEXCOORD, gfxDst);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 256.0 / 192.0, 0.1, 40);

    gluLookAt(0.0, 0.0, 2.0,  // Position
              0.0, 0.0, 0.0,  // Look at
              0.0, 1.0, 0.0); // Up

    int angle_x = 0;
    int angle_z = 0;

    while (1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        // Print some text in the demo console
        // -----------------------------------

        // Clear console
        printf("\x1b[2J");

        // Print some controls
        printf("PAD:    Rotate triangle\n");
        printf("START:  Exit to loader\n");
        printf("\n");

        // Handle user input
        // -----------------

        scanKeys();

        uint16_t keys = keysHeld();

        if (keys & KEY_LEFT)
            angle_z += 3;
        if (keys & KEY_RIGHT)
            angle_z -= 3;

        if (keys & KEY_UP)
            angle_x += 3;
        if (keys & KEY_DOWN)
            angle_x -= 3;

        if (keys & KEY_START)
            break;

        // Render 3D scene
        // ---------------

        glMatrixMode(GL_MODELVIEW);

        glPushMatrix();

        glRotateZ(angle_z);
        glRotateX(angle_x);

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

        glBindTexture(0, textureID);

        glColor3f(1, 1, 1);

        glBegin(GL_QUADS);

            GFX_TEX_COORD = (TEXTURE_PACK(0, inttot16(128)));
            glVertex3v16(floattov16(-1), floattov16(-1), 0);

            GFX_TEX_COORD = (TEXTURE_PACK(inttot16(128),inttot16(128)));
            glVertex3v16(floattov16(1), floattov16(-1), 0);

            GFX_TEX_COORD = (TEXTURE_PACK(inttot16(128), 0));
            glVertex3v16(floattov16(1), floattov16(1), 0);

            GFX_TEX_COORD = (TEXTURE_PACK(0,0));
            glVertex3v16(floattov16(-1), floattov16(1), 0);

        glEnd();

        glPopMatrix(1);

        glFlush(0);
    }

    return 0;
}
