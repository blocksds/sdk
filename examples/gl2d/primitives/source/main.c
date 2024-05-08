// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <gl2d.h>
#include <nds.h>

const uint32_t screen_width = 256;
const uint32_t screen_height = 192;

const uint32_t screen_half_width = screen_width / 2;
const uint32_t screen_half_height = screen_height / 2;

int main(int argc, char **argv)
{
    consoleDemoInit();

    videoSetMode(MODE_0_3D);

    // Initialize OpenGL to some sensible defaults
    glScreen2D();

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

            // Top left quadrant of the screen

            glBoxFilledGradient(0, 0,
                                screen_half_width - 1, screen_half_height - 1,
                                RGB15(31, 0, 0),
                                RGB15(31, 31, 0),
                                RGB15(31, 0, 31),
                                RGB15(0, 31, 31));

            // Top right quadrant of the screen

            glBoxFilled(screen_half_width, 0,
                        screen_width - 1, screen_half_height - 1,
                        RGB15(10, 10, 10));

            glBox(screen_half_width, 0,
                  screen_width - 1, screen_half_height - 1,
                  RGB15(20, 20, 20));

            // Bottom left quadrant of the screen

            glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(1));

            glTriangleFilled(0, screen_height * 3 / 4,
                             screen_half_width - 1, screen_half_height,
                             screen_half_width - 1, screen_height - 1,
                             RGB15(31, 31, 31));

            glPolyFmt(POLY_ALPHA(16) | POLY_CULL_NONE | POLY_ID(2));

            glTriangleFilledGradient(0, screen_half_height,
                                     screen_half_width - 1, screen_height * 3 / 4,
                                     0, screen_height - 1,
                                     RGB15(0, 31, 0),
                                     RGB15(31, 0, 0),
                                     RGB15(0, 0, 31));

            glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(3));

            // Bottom right quadrant of the screen

            // Note that the pixels can't be seen in some emulators
            glPutPixel(screen_width * 3 / 4 + 10, screen_height * 3 / 4,
                       RGB15(0, 31, 31));

            glPutPixel(screen_width * 3 / 4 - 10, screen_height * 3 / 4,
                       RGB15(0, 31, 31));

            glLine(screen_half_width, screen_half_height,
                   screen_width, screen_height,
                   RGB15(31, 0, 0));

            glLine(screen_width, screen_half_height,
                   screen_half_width, screen_height,
                   RGB15(0, 31, 0));

        glEnd2D();

        glFlush(0);
    }

    return 0;
}
