// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Adrian "asie" Siekierka, 2024

#include <stdio.h>
#include <math.h>
#include <nds.h>
#include <gl2d.h>

#define TOUCH_BOX_RADIUS 4

int main(int argc, char **argv)
{
    uint16_t keys_held;
    touchPosition touch_pos;

    // Initialize display:
    // - sub screen (top): console output
    // - main screen (bottom): GL2D graphics
    consoleDemoInit();
    lcdSwap();

    videoSetMode(MODE_0_3D);
    glScreen2D();

    while (1)
    {
        swiWaitForVBlank();

        // Read key input.
        scanKeys();
        keys_held = keysHeld();

        // Handle key presses.
        if ((keys_held & (KEY_START | KEY_SELECT)) == (KEY_START | KEY_SELECT))
            break;

        // If pen down, update touch input.
        if (keys_held & KEY_TOUCH)
            touchRead(&touch_pos);

        // Print touch position information to console.
        printf("\x1b[2J");
        printf("Touch input example\nPress START+SELECT to exit\n\n");

        if (keys_held & KEY_TOUCH)
            printf("\x1b[32;1m");
        else
            printf("\x1b[31;1m");

        printf("Raw coords: [%d, %d]\n", touch_pos.rawx, touch_pos.rawy);
        printf("Adjusted coords: [%d, %d]\n", touch_pos.px, touch_pos.py);
        printf("\x1b[39;0m");

        // Draw a square on the bottom screen where the user is touching.
        glBegin2D();

        if (keys_held & KEY_TOUCH)
        {
            glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(0));

            glBoxFilled(
                touch_pos.px - TOUCH_BOX_RADIUS,
                touch_pos.py - TOUCH_BOX_RADIUS,
                touch_pos.px + TOUCH_BOX_RADIUS,
                touch_pos.py + TOUCH_BOX_RADIUS,
                RGB15(31, 31, 31)
            );
        }

        glEnd2D();
        glFlush(0);
    }

    return 0;
}

