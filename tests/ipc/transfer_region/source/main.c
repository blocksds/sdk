// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

// This tests whether the ARM7<->ARM9 transfer region area works correctly. It
// is used so that the ARM7 can pass the ARM9 information like the current time
// and date, the state of some buttons, and the state of the touch screen.
//
// The transfer region is located in a different address in DS and DSi, this
// test can be used to check if it's still working.

#include <stdio.h>
#include <time.h>

#include <nds.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        uint16_t keys_held = keysHeld();

        consoleClear();

        // Print current time
        char str[100];
        time_t t = time(NULL);
        struct tm *tmp = localtime(&t);
        if (strftime(str, sizeof(str), "%Y-%m-%dT%H:%M:%S%z", tmp) == 0)
            snprintf(str, sizeof(str), "Failed to get time");
        printf("%s\n\n", str);

        // Print button status

        printf("Buttons:\n");
        printf("  X:     %d\n", (keys_held & KEY_X) ? 1 : 0);
        printf("  Y:     %d\n", (keys_held & KEY_Y) ? 1 : 0);
        printf("  Touch: %d\n", (keys_held & KEY_TOUCH) ? 1 : 0);
        printf("  Lid:   %d\n", (keys_held & KEY_LID) ? 1 : 0);
        printf("  Debug: %d\n", (keys_held & KEY_DEBUG) ? 1 : 0);
        printf("\n");

        // Print touch screen information

        unsigned int rawx = 0, rawy = 0, px = 0, py = 0, area = 0;

        if (keys_held & KEY_TOUCH)
        {
            touchPosition touch;
            touchRead(&touch);

            if (touch.z1 != 0)
                area = touch.px * touch.z2 / touch.z1 - touch.px;

            rawx = touch.rawx;
            rawy = touch.rawy;
            px = touch.px;
            py = touch.py;
        }

        printf("Touch screen:\n");
        printf("  X = %3u (%u)\n", px, rawx);
        printf("  Y = %3u (%u)\n", py, rawy);
        printf("  Area/Pressure: %u\n", area);
        printf("\n");

        // Print button information

        printf("Press START to exit\n");

        if (keys_held & KEY_START)
            break;
    }

    return 0;
}
