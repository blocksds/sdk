// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// This example shows how to implement an FPS counter. It should normally be
// 59-60 FPS, but it will go down to 40 FPS when the user presses button A and
// it hangs the main loop.

#include <stdio.h>

#include <nds.h>

int fps;
int irq_frame_count;

void timer0_handler(void)
{
    fps = irq_frame_count;
    irq_frame_count = 0;
}

int main(int argc, char **argv)
{
    // Timer 0 will be called every second
    timerStart(0, ClockDivider_1024, timerFreqToTicks_1024(1), timer0_handler);

    consoleDemoInit();

    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("A: Hang code for 20 frames\n");
    printf("START: Exit to loader\n");

    while (1)
    {
        irq_frame_count++;
        swiWaitForVBlank();

        consoleSetCursor(NULL, 0, 0);
        printf("FPS: %d  ", fps);

        scanKeys();

        if (keysDown() & KEY_A)
        {
            for (int i = 0; i < 20; i++)
                swiWaitForVBlank();
        }

        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
