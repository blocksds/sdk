// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <gl2d.h>
#include <nds.h>

#include "tiny_16.h"

#define MAP_WIDTH   30
#define MAP_HEIGHT  20

const int16_t map[MAP_WIDTH * MAP_HEIGHT] = {
    94, 95, 84, 85, 94, 95, 84, 85, 94, 95, 1, 1, 1, 1, 1, 1, 1, 1, 94, 95, 84,
    85, 84, 85, 94, 95, 84, 85, 84, 85, 84, 85, 94, 95, 84, 85, 94, 95, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 94, 95, 94, 95, 84, 85, 94, 95, 94, 95, 94, 95,
    84, 85, 94, 95, 1, 0, 1, 1, 1, 1, 1, 1, 7, 7, 7, 1, 0, 1, 1, 1, 1, 1, 94,
    95, 84, 85, 84, 85, 1, 1, 94, 95, 1, 1, 1, 7, 1, 1, 1, 1, 1, 1, 1, 0, 2, 1,
    1, 1, 1, 1, 1, 1, 7, 7, 94, 95, 94, 95, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 7,
    1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 84, 85, 1, 1, 7, 0, 1, 1, 1,
    1, 1, 1, 7, 2, 1, 1, 1, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 94, 95, 1, 7,
    7, 7, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 47, 48, 48, 48, 49, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 84, 0, 17, 18, 18, 18, 18, 18, 18, 19, 1, 1, 1, 1, 1, 1, 57, 58,
    58, 58, 59, 1, 1, 1, 1, 0, 1, 1, 1, 1, 94, 0, 27, 1, 1, 1, 1, 1, 1, 29, 1,
    1, 1, 1, 1, 7, 67, 68, 68, 68, 69, 1, 1, 7, 7, 2, 1, 7, 1, 84, 85, 7, 27, 1,
    1, 1, 1, 1, 1, 29, 1, 1, 1, 1, 1, 1, 87, 97, 98, 99, 89, 1, 1, 0, 7, 7, 0,
    0, 1, 94, 95, 7, 27, 1, 1, 1, 1, 1, 1, 29, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 0, 1, 1, 1, 84, 85, 84, 0, 27, 1, 1, 1, 1, 1, 1, 29, 7, 0, 1, 1, 1,
    1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 94, 95, 94, 7, 27, 1, 1, 1, 1, 1, 1,
    29, 2, 7, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 84, 85, 1, 37,
    38, 38, 38, 38, 28, 38, 39, 7, 7, 1, 1, 1, 1, 20, 21, 21, 21, 21, 21, 21,
    21, 22, 1, 1, 1, 1, 94, 95, 7, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 7, 1, 1, 1, 30,
    31, 31, 31, 31, 31, 31, 31, 32, 1, 1, 1, 1, 84, 85, 0, 7, 1, 1, 1, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 20, 44, 31, 31, 31, 31, 31, 31, 31, 43, 22, 1, 2, 1, 94,
    95, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 30, 31, 31, 31, 31, 31, 31,
    31, 31, 31, 32, 1, 7, 1, 1, 84, 1, 1, 1, 1, 1, 1, 1, 1, 7, 1, 1, 1, 1, 20,
    44, 31, 31, 31, 31, 31, 31, 31, 31, 31, 32, 1, 1, 1, 1, 94, 1, 1, 1, 0, 1,
    1, 1, 1, 1, 1, 0, 1, 1, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 43, 22,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 20, 44, 31, 31, 31, 31, 31,
    31, 31, 31, 31, 31, 31, 32, 1, 1, 1, 1
};

glImage tileset[10 * 10];

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
    int tileset_texture_id =
        glLoadTileSet(tileset,      // Pointer to glImage array
                      16,           // Sprite width
                      16,           // Sprite height
                      16 * 10,      // Bitmap width (the part that contains useful images)
                      16 * 10,      // Bitmap height (the part that contains useful images)
                      GL_RGB256,    // Texture type for glTexImage2D()
                      256,          // Full texture size X (image size)
                      256,          // Full texture size Y (image size)
                      TEXGEN_TEXCOORD, // Parameters for glTexImage2D()
                      256,            // Length of the palette to use (256 colors)
                      tiny_16Pal,     // Pointer to texture palette data
                      tiny_16Bitmap); // Pointer to texture data

    if (tileset_texture_id < 0)
        printf("Failed to load texture: %d\n", tileset_texture_id);

    // Print some controls
    printf("PAD:    Scroll\n");
    printf("START:  Exit to loader\n");
    printf("\n");

    int scroll_x = 0;
    int scroll_y = 0;

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

        if (keys & KEY_UP)
            scroll_y++;
        if (keys & KEY_DOWN)
            scroll_y--;

        if (keys & KEY_LEFT)
            scroll_x++;
        if (keys & KEY_RIGHT)
            scroll_x--;

        // Render 3D scene
        // ---------------

        // Set up GL2D for 2D mode
        glBegin2D();

            glColor(RGB15(31, 31, 31));
            glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);

            // This code could be made more intelligent by only drawing the
            // tiles that are actually shown on the screen. That would reduce
            // the number of polygons that are sent to the GPU and improve
            // performance.
            for (int j = 0; j < MAP_HEIGHT; j++)
            {
                for (int i = 0; i < MAP_WIDTH; i++)
                {
                    int x = scroll_x + i * 16;
                    int y = scroll_y + j * 16;
                    int tile_id = map[j * MAP_WIDTH + i];

                    glSprite(x, y, GL_FLIP_NONE, &tileset[tile_id]);
                }
            }

        glEnd2D();

        glFlush(0);
    }

    glDeleteTextures(1, &tileset_texture_id);

    return 0;
}
