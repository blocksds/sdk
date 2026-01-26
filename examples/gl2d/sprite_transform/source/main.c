// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

// This example shows the artifacts that appear when scaling and rotating
// sprites with GL2D.

#include <stdio.h>

#include <gl2d.h>
#include <nds.h>

#include "s8.h"
#include "s16.h"
#include "s64.h"

const uint32_t screen_width = 256;
const uint32_t screen_height = 192;

glImage texture[3];

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Initialize OpenGL to some sensible defaults
    glScreen2D();

    videoSetMode(MODE_0_3D);

    // Setup some memory to be used for textures and for texture palettes
    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankE(VRAM_E_TEX_PALETTE);

    int texture_id[3];

    texture_id[0]=
        glLoadTileSet(&texture[0], // Pointer to glImage array
                      8, 8,     // Tile size
                      8, 8,     // Bitmap size
                      GL_RGB16, // Texture type
                      8, 8,     // GL texture size
                      // Texture parameters
                      TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT,
                      16,        // Length of the palette to use
                      s8Pal,     // Pointer to texture palette data
                      s8Bitmap); // Pointer to texture data

    if (texture_id[0] < 0)
        printf("Failed to load texture 0: %d\n", texture_id[0]);

    texture_id[1]=
        glLoadTileSet(&texture[1], // Pointer to glImage array
                      16, 16,   // Tile size
                      16, 16,   // Bitmap size
                      GL_RGB16, // Texture type
                      16, 16,   // GL texture size
                      // Texture parameters
                      TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT,
                      16,         // Length of the palette to use
                      s16Pal,     // Pointer to texture palette data
                      s16Bitmap); // Pointer to texture data

    if (texture_id[1] < 0)
        printf("Failed to load texture 1: %d\n", texture_id[1]);

    texture_id[2]=
        glLoadTileSet(&texture[2], // Pointer to glImage array
                      64, 64,   // Tile size
                      64, 64,   // Bitmap size
                      GL_RGB16, // Texture type
                      64, 64,   // GL texture size
                      // Texture parameters
                      TEXGEN_TEXCOORD | GL_TEXTURE_COLOR0_TRANSPARENT,
                      16,         // Length of the palette to use
                      s64Pal,     // Pointer to texture palette data
                      s64Bitmap); // Pointer to texture data

    if (texture_id[2] < 0)
        printf("Failed to load texture 2: %d\n", texture_id[2]);

    // Print some controls
    printf("L/R:     Rotate\n");
    printf("Up/Down: Scale up/down\n");
    printf("A/B:   Flip horizontal/vertical\n");
    printf("X:     Set scale to 100%%\n");
    printf("Y:     Set scale to 200%%\n");
    printf("START: Exit to loader\n");
    printf("\n");

    s32 angle = 0;
    s32 scale = 2 << 12;
    int flipmode = GL_FLIP_NONE;

    while (1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        // Handle user input
        // -----------------

        scanKeys();

        uint16_t keys_down = keysDown();
        uint16_t keys = keysHeld();
        if (keys & KEY_START)
            break;

        if (keys & KEY_L)
            angle -= 1 << 5;
        if (keys & KEY_R)
            angle += 1 << 5;

        if (keys & KEY_UP)
            scale += 1 << 5;
        if (keys & KEY_DOWN)
            scale -= 1 << 5;

        if (keys_down & KEY_A)
            flipmode ^= GL_FLIP_H;
        if (keys_down & KEY_B)
            flipmode ^= GL_FLIP_V;

        if (keys & KEY_X)
        {
            angle = 0;
            scale = 1 << 12;
            flipmode = GL_FLIP_NONE;
        }
        if (keys & KEY_Y)
        {
            angle = 0;
            scale = 2 << 12;
            flipmode = GL_FLIP_NONE;
        }

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

            glSprite(224, 92, flipmode, &texture[1]);

            glSpriteRotateScale(80, 92,
                                angle, scale, flipmode, &texture[2]);
            glSpriteRotateScale(192, 128,
                                angle, scale, flipmode, &texture[1]);
            glSpriteRotateScale(192, 64,
                                angle, scale, flipmode, &texture[0]);

        glEnd2D();

        glFlush(0);
    }

    glDeleteTextures(3, &texture_id[0]);

    return 0;
}
