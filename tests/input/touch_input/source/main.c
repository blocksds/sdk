// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Adrian "asie" Siekierka, 2024

#include <stdio.h>
#include <math.h>
#include <nds.h>
#include <gl2d.h>

#define TOUCH_BOX_RADIUS 3
#define TOUCH_POS_INVALID 0xFFFF

// Define a rolling buffer for X/Y variables.
#define ROLLING_BUFFER_SIZE 32
u16 rolling_buffer_x[ROLLING_BUFFER_SIZE];
u16 rolling_buffer_y[ROLLING_BUFFER_SIZE];
u16 rolling_buffer_pos = 0;

typedef enum
{
    ScreenModeRaw,
    ScreenModeAdj,
    ScreenModePressure,
    ScreenModeCount
} screenMode;

typedef enum
{
    LatchOnHold,
    LatchOnPress,
    LatchOnCount
} latchOn;

void rolling_buffer_add(u16 x, u16 y)
{
    rolling_buffer_x[rolling_buffer_pos] = x;
    rolling_buffer_y[rolling_buffer_pos] = y;
    rolling_buffer_pos = (rolling_buffer_pos + 1) % ROLLING_BUFFER_SIZE;
}

void rolling_buffer_reset(void)
{
    memset(rolling_buffer_x, 0xFF, sizeof(rolling_buffer_x));
    memset(rolling_buffer_y, 0xFF, sizeof(rolling_buffer_y));
    rolling_buffer_pos = 0;
}

int main(int argc, char **argv)
{
    uint16_t keys_down, keys_held;
    touchPosition touch_pos;
    screenMode screen_mode = ScreenModeRaw;
    latchOn latch_on = LatchOnHold;

    rolling_buffer_reset();

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
        keys_down = keysDown();
        keys_held = keysHeld();

        bool touch_cond = false;
        if (latch_on == LatchOnHold) touch_cond = keys_held & KEY_TOUCH;
        if (latch_on == LatchOnPress) touch_cond = keys_down & KEY_TOUCH;

        if (touch_cond)
            touchRead(&touch_pos);

        // Print touch position information to console.
        printf("\x1b[2J");
        printf("Touch screen test code\n\n");
        if (screen_mode == ScreenModeRaw) printf("Screen mode: Raw input\n");
        if (screen_mode == ScreenModeAdj) printf("Screen mode: Adjusted input\n");
        if (screen_mode == ScreenModePressure) printf("Screen mode: Pressure\n");
        printf("\x1b[37;0mPress LEFT/RIGHT to switch\n\n");
        if (latch_on == LatchOnHold) printf("Latch on: Hold\n");
        if (latch_on == LatchOnPress) printf("Latch on: Press\n");
        printf("\x1b[37;0mPress UP/DOWN to switch\n\nPress START+SELECT to exit\n\n");

        if (touch_cond)
            printf("\x1b[32;1m");
        else
            printf("\x1b[31;1m");

        printf("Raw coords: [%d, %d]\n", touch_pos.rawx, touch_pos.rawy);
        printf("Adj coords: [%d, %d]\n", touch_pos.px, touch_pos.py);
        printf("Touch Z1Z2: [%d, %d]\n", touch_pos.z1, touch_pos.z2);
        printf("\x1b[39;0m");

        // Handle key presses.
        if ((keys_held & (KEY_START | KEY_SELECT)) == (KEY_START | KEY_SELECT))
            break;

        if (keys_down & KEY_LEFT)
        {
            rolling_buffer_reset();
            screen_mode = (screen_mode > 0 ? screen_mode : ScreenModeCount) - 1;
        }
        if (keys_down & KEY_RIGHT)
        {
            rolling_buffer_reset();
            screen_mode = (screen_mode + 1) % ScreenModeCount;
        }
        if (keys_down & KEY_UP)
        {
            rolling_buffer_reset();
            latch_on = (latch_on > 0 ? latch_on : LatchOnCount) - 1;
        }
        if (keys_down & KEY_DOWN)
        {
            rolling_buffer_reset();
            latch_on = (latch_on + 1) % LatchOnCount;
        }

        // Update rolling buffer.
        if (keys_held & KEY_TOUCH)
        {
            if (screen_mode == ScreenModeRaw) rolling_buffer_add(touch_pos.rawx, touch_pos.rawy);
            if (screen_mode == ScreenModeAdj) rolling_buffer_add(touch_pos.px, touch_pos.py);
        }
        else
        {
            rolling_buffer_add(TOUCH_POS_INVALID, TOUCH_POS_INVALID);
        }

        glBegin2D();

        if (screen_mode == ScreenModePressure)
        {
            glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(1));

            // Draw/print pressure data.
            if ((keys_held & KEY_TOUCH) && touch_pos.z1 != 0)
            {
                const u32 maxPressure = 0x00DE0000;
                const u32 maxRadius = 128;

                u32 tosend = touch_pos.rawx * ((touch_pos.z2 * 4096 / touch_pos.z1) - 4096);
                float pressure = (float) maxPressure / (float) tosend * maxRadius;

                printf("\n\x1b[33;1m");
                printf("Resistance: %lu\n\x1b[33;0m            %08lX\x1b[33;1m\n", tosend, tosend);
                printf("Pressure:   %f\n", pressure);
                printf("\x1b[39;0m");

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

                        // Darken box color.
                        shapeColor = ((shapeColor >> 1) & 0x3DEF) | 0x8000;
                    }
                }
            }
        }
        else
        {
            // Draw rolling buffer (modes raw, adjusted).
            for (int i = 0; i < ROLLING_BUFFER_SIZE; i++)
            {
                glPolyFmt(POLY_ALPHA(3) | POLY_CULL_NONE | POLY_ID(i));

                u32 bx = rolling_buffer_x[i];
                u32 by = rolling_buffer_y[i];
                if (bx == TOUCH_POS_INVALID || by == TOUCH_POS_INVALID)
                    continue;

                if (screen_mode == ScreenModeRaw)
                {
                    // Downscale raw coordinates from 0..4095 to 0..255 and 0..191.
                    bx = (bx * 255) / 4095;
                    by = (by * 191) / 4095;
                }

                glBoxFilled(
                    bx - TOUCH_BOX_RADIUS,
                    by - TOUCH_BOX_RADIUS,
                    bx + TOUCH_BOX_RADIUS,
                    by + TOUCH_BOX_RADIUS,
                    RGB15(31, 31, 31)
                );
            }
        }

        glEnd2D();
        glFlush(0);
    }

    return 0;
}

