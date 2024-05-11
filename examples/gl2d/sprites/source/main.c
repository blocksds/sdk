// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <gl2d.h>
#include <nds.h>

#include "advnt.h"

const uint32_t screen_width = 256;
const uint32_t screen_height = 192;

glImage character[6];

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Initialize OpenGL to some sensible defaults
    glScreen2D();

    videoSetMode(MODE_0_3D);

    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankE(VRAM_E_TEX_PALETTE);

    int character_texture_id =
        glLoadTileSet(character,    // Pointer to glImage array
                      22,           // Sprite width
                      64,           // Sprite height
                      22 * 6,       // Bitmap width (the part that contains useful images)
                      64,           // Bitmap height (the part that contains useful images)
                      GL_RGB256,    // Texture type for glTexImage2D()
                      256,          // Full texture size X (image size)
                      64,           // Full texture size Y (image size)
                      // Parameters for glTexImage2D()
                      TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT,
                      256,          // Length of the palette to use (256 colors)
                      advntPal,     // Pointer to texture palette data
                      advntBitmap); // Pointer to texture data

    int delay = 0;
    int frame = 0;

    while (1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        // Print some text in the demo console
        // -----------------------------------

        // Clear console
        printf("\x1b[2J");

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

        // Set up GL2D for 2D mode
        glBegin2D();

            // Fill screen
            glBoxFilledGradient(0, 0,
                                screen_width - 1, screen_height - 1,
                                RGB15(31, 0, 0),
                                RGB15(31, 31, 0),
                                RGB15(31, 0, 31),
                                RGB15(0, 31, 31));

            // Draw sprite frames
            glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(0));
            glColor(RGB15(31, 31, 31));

            for (unsigned int i = 0; i < 6; i++)
            {
                unsigned int x = i * 24;
                unsigned int y = i * 5;
                glSprite(x, y, GL_FLIP_NONE, &character[i]);
            }

            glSprite(200, 0, GL_FLIP_NONE, &character[frame]);

            // Animate (change animation frame every 10 frames)
            delay++;
            if (delay == 10)
            {
                delay = 0;

                frame++;
                if (frame == 6)
                    frame = 0;
            }

        glEnd2D();

        glFlush(0);
    }

    glDeleteTextures(1, &character_texture_id);

    return 0;
}
