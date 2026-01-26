// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <gl2d.h>
#include <nds.h>

#include "atlas_texture.h"
#include "atlas.h"

const uint32_t screen_width = 256;
const uint32_t screen_height = 192;

glImage ruins[ATLAS_NUM_IMAGES];

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Initialize OpenGL to some sensible defaults
    glScreen2D();

    videoSetMode(MODE_0_3D);

    // Setup some memory to be used for textures and for texture palettes
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankE(VRAM_E_TEX_PALETTE);

    // A sprite set is one big texture formed by several images of different
    // sizes. The location and size of each image is stored in
    // "ATLAS_texcoords", where "ATLAS" is a string defined by you when packing
    // the images.
    //
    // The GPU of the DS requires textures to have sizes that are power
    // of two. When you have a bitmap with dimensions that aren't a power of
    // two, padding needs to be added to the bottom and to the right to fill the
    // image up to a valid size.
    //
    // Combining multiple non-power-of-two images into a single power-of-two
    // texture allows you to use VRAM more efficiently.
    int ruins_texture_id =
        glLoadSpriteSet(ruins,    // Pointer to glImage array
                        ATLAS_NUM_IMAGES, // Number of images
                        ATLAS_texcoords,  // Array of UV coordinates
                        GL_RGB256,        // Texture type
                        ATLAS_BITMAP_WIDTH,  // Full texture size X (image size)
                        ATLAS_BITMAP_HEIGHT, // Full texture size Y (image size)
                        // Texture parameters
                        TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT,
                        256,          // Length of the palette to use (256 colors)
                        atlas_texturePal,     // Pointer to texture palette data
                        atlas_textureBitmap); // Pointer to texture data

    if (ruins_texture_id < 0)
        printf("Failed to load texture: %d\n", ruins_texture_id);

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

            glSprite(0, 4, GL_FLIP_NONE, &ruins[ATLAS_1_png]);
            glSprite(64, 4, GL_FLIP_NONE, &ruins[ATLAS_2_png]);
            glSprite(128, 4, GL_FLIP_NONE, &ruins[ATLAS_3_png]);
            glSprite(192, 4, GL_FLIP_NONE, &ruins[ATLAS_4_png]);
            glSprite(0, 100, GL_FLIP_NONE, &ruins[ATLAS_5_png]);
            glSprite(80, 100, GL_FLIP_NONE, &ruins[ATLAS_6_png]);
            glSprite(180, 100, GL_FLIP_NONE, &ruins[ATLAS_7_png]);

            // Draw animated sprite
            glSprite(80, 60, GL_FLIP_NONE, &ruins[frame]);

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

    glDeleteTextures(1, &ruins_texture_id);

    return 0;
}
