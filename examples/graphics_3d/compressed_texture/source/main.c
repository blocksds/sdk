// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <nds.h>

#include "neon_tex_bin.h"
#include "neon_pal_bin.h"
#include "neon_idx_bin.h"

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
    int textureID;

    // Setup sub screen for the text console
    consoleDemoInit();

    videoSetMode(MODE_0_3D);

    glInit();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ANTIALIAS);

    glClearColor(0, 0, 0, 31);
    glClearPolyID(63);

    glClearDepth(0x7FFF);

    glViewport(0, 0, 255, 191);

    // Setup some VRAM as memory for textures. Note that compressed textures
    // need space in bank B and one of A or C.
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    // Setup some VRAM as memory for texture palettes.
    vramSetBankE(VRAM_E_TEX_PALETTE);

    // Load texture
    glGenTextures(1, &textureID);
    glBindTexture(0, textureID);
    {
        // glTexImage2D() expects the "tex" and "idx" parts of the texture to be
        // placed together. ptexconv generates different files for them (and
        // another file for the palette) so it is needed to concatenate them.
        uint8_t *buffer = malloc(neon_tex_bin_size + neon_idx_bin_size);
        if (buffer == NULL)
        {
            printf("Failed to allocate memory");
            while (1)
                swiWaitForVBlank();
        }
        memcpy(buffer, neon_tex_bin, neon_tex_bin_size);
        memcpy(buffer + neon_tex_bin_size, neon_idx_bin, neon_idx_bin_size);

        if (glTexImage2D(0, 0, GL_COMPRESSED, 128, 128, 0, TEXGEN_TEXCOORD, buffer) == 0)
        {
            printf("Failed to load texture\n");
            wait_forever();
        }
        if (glColorTableEXT(0, 0, neon_pal_bin_size / 2, 0, 0, neon_pal_bin) == 0)
        {
            printf("Failed to load palette\n");
            wait_forever();
        }

        free(buffer);
    }

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
        printf("PAD:    Rotate quad\n");
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
