// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

int timer0_count = 0;
int timer1_count = 0;

void timer0_handler(void)
{
    timer0_count++;
}

void timer1_handler(void)
{
    timer1_count++;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Timer 0 will be called every second. This is a long time to wait, so a
    // big divider is required so that the timer counter can fit enough ticks to
    // count up to a second.
    //
    // Timer 1 will be called five times per second. The total number of ticks
    // is smaller, so a smaller divider can be used.
    timerStart(0, ClockDivider_1024, timerFreqToTicks_1024(1), timer0_handler);
    timerStart(1, ClockDivider_256, timerFreqToTicks_256(5), timer1_handler);

    while (1)
    {
        swiWaitForVBlank();

        // Clear console
        printf("\x1b[2J");
        printf("Timer 0: %d\n", timer0_count);
        printf("Timer 1: %d\n", timer1_count);
        printf("\n");
        printf("Press START to exit");

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
