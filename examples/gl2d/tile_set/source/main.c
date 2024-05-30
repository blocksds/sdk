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

    // Setup some memory to be used for textures and for texture palettes
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankE(VRAM_E_TEX_PALETTE);

    // A tile set is formed by several images of the same size that start at the
    // top left corner. It increses to the right in the top row until the end of
    // the texture is reached, then it continues to the second row.
    //
    // When all the images are put together they form a bitmap with some
    // dimensions. The dimensions can be whatever is required for that specific
    // sprite, with no restrictions.
    //
    // However, the GPU of the DS requires textures to have sizes that are power
    // of two. When you have a bitmap with dimensions that aren't a power of
    // two, padding needs to be added to the bottom and to the right to fill the
    // image up to a valid size.
    //
    // Note that if you leave enough space on the right of the texture for a new
    // image, even if there aren't graphics there, it will count.
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

            for (int i = 0; i < 6; i++)
            {
                int x = i * 24;
                int y = i * 5;
                glSprite(x, y, GL_FLIP_NONE, &character[i]);
            }

            // Draw animated sprite
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

            // Draw a rotated and scaled sprite
            glSpriteRotateScaleXY(40, 150, // Position
                    32767 / 8, // Rotation = 45 degrees (full circle / 8)
                    floattof32(1.2), floattof32(0.8), // Scale X and Y
                    GL_FLIP_NONE, &character[0]);

        glEnd2D();

        glFlush(0);
    }

    glDeleteTextures(1, &character_texture_id);

    return 0;
}
