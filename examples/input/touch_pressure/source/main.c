// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Adrian "asie" Siekierka, 2024

#include <stdio.h>
#include <math.h>
#include <nds.h>
#include <gl2d.h>

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
        printf("Touch pressure example\nPress START+SELECT to exit\n\n");

        if (keys_held & KEY_TOUCH)
            printf("\x1b[32;1m");
        else
            printf("\x1b[31;1m");

        printf("Raw coords: [%d, %d]\n", touch_pos.rawx, touch_pos.rawy);
        printf("Adj coords: [%d, %d]\n", touch_pos.px, touch_pos.py);
        printf("Touch Z1Z2: [%d, %d]\n", touch_pos.z1, touch_pos.z2);
        printf("\x1b[39;0m");

        glBegin2D();

        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(1));

        // Calculate the amount of pressure applied on the screen.
        // TODO: This should probably be a fixed point calculation.
        float pressure = INFINITY;
        const u32 maxRadius = 128;

        // Make sure to check if z1 is non-zero. Some configurations
        // (DSi/3DS consoles running in NDS mode) do not support
        // pressure measurements at all.
        if ((keys_held & KEY_TOUCH) && touch_pos.z1 != 0)
        {
            const u32 maxPressure = 0x00DE0000;

            u32 resistance = touch_pos.rawx * ((touch_pos.z2 * 4096 / touch_pos.z1) - 4096);
            pressure = (float) maxPressure / (float) resistance * maxRadius;

            printf("\n\x1b[33;1m");
            printf("Resistance: %lu\n\x1b[33;0m            %08lX\x1b[33;1m\n", resistance, resistance);
            printf("Pressure:   %f\n", pressure);
            printf("\x1b[39;0m");
        }

        // Draw consecutive boxes to reflect pressure measurement.
        if (!isinf(pressure) && pressure > 0)
        {
            u32 shapeColor = RGB8(146, 77, 200) | 0x8000;
            u32 maxPixelsPerShape = 192;
            float currentPressure = pressure;

            while (currentPressure > 0)
            {
                u32 radius = (currentPressure * currentPressure) / maxRadius;
                glBoxFilled(
                    touch_pos.px - radius,
                    touch_pos.py - radius,
                    touch_pos.px + radius,
                    touch_pos.py + radius,
                    shapeColor
                );
                currentPressure -= maxPixelsPerShape;

                // Darken next box color.
                shapeColor = ((shapeColor >> 1) & 0x3DEF) | 0x8000;
            }
        }

        glEnd2D();
        glFlush(0);
    }

    return 0;
}

