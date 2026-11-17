// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

#include <stdio.h>

#include <nds.h>

volatile int timer1_count;
volatile int timer2_count;
volatile bool success;

void timer0_handler(void)
{
    REG_IME = 1;

    while ((timer1_count < 10) && (timer2_count < 100));

    success = true;
}

void timer1_handler(void)
{
    timer1_count++;
}

void timer2_handler(void)
{
    timer2_count++;
}

int main(int argc, char **argv)
{
    defaultExceptionHandler();

    consoleDemoInit();

    consoleDebugInit(DebugDevice_NOCASH);

    printf("Nested IRQs test\n");
    printf("----------------\n");
    printf("\n");
    printf("Press START to exit to loader\n");
    printf("\n");

    // Call timer 0 once per second
    timerStart(0, ClockDivider_1024, timerFreqToTicks_1024(1), timer0_handler);

    // Setup timer 1 to fire 10 times per second
    timerStart(1, ClockDivider_1024, TIMER_FREQ_1024(10), timer1_handler);

    // Setup timer 2 to fire 100 times per second
    timerStart(2, ClockDivider_1, TIMER_FREQ_1024(100), timer2_handler);

    while (1)
    {
        swiWaitForVBlank();

        if (success)
        {
            printf("Success\n");
            fprintf(stderr, "[TEST] Success\n");
            timerStop(0);
            timerStop(1);
            timerStop(2);
            success = false;
        }

        scanKeys();

        uint32_t keys = keysHeld();
        if (keys & KEY_START)
            break;
    }

    return 0;
}
