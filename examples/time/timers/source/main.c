// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2024

#include <stdio.h>
#include <time.h>

#include <nds.h>

int timer0_count = 0;
int timer1_count = 0;
int timer2_count = 0;
int timer3_count = 0;

int timer2_count_per_second = 0;
int timer3_count_per_second = 0;

void timer0_handler(void)
{
    timer0_count++;

    timer2_count_per_second = timer2_count;
    timer2_count = 0;
    timer3_count_per_second = timer3_count;
    timer3_count = 0;
}

void timer1_handler(void)
{
    timer1_count++;
}

void timer2_handler(void)
{
    timer2_count++;
}

void timer3_handler(void)
{
    timer3_count++;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Timer 0 will be called every second. This is a long time to wait, so a
    // big divider is required so that the timer counter can fit enough ticks to
    // count up to a second.
    timerStart(0, ClockDivider_1024, timerFreqToTicks_1024(1), timer0_handler);

    // Timer 1 will be called five times per second. The total number of ticks
    // is smaller, so a smaller divider can be used.
    timerStart(1, ClockDivider_256, timerFreqToTicks_256(5), timer1_handler);

#define TIMER_2_TARGET 1500
#define TIMER_3_TARGET 300

    // Setup timer 2 to fire 1500 times per second, and timer 3 to fire 300
    // times per second. Timer 0 will capture the number of times timers 2 and 3
    // have fired so that we can check if it's happening at the expected rate.
    timerStart(2, ClockDivider_1, TIMER_FREQ(TIMER_2_TARGET), timer2_handler);
    timerStart(3, ClockDivider_64, TIMER_FREQ_64(TIMER_3_TARGET), timer3_handler);

    while (1)
    {
        swiWaitForVBlank();

        consoleClear();

        // Print current time
        char str[100];
        time_t t = time(NULL);
        struct tm *tmp = localtime(&t);
        printf("Current time:\n\n");
        if (strftime(str, sizeof(str), "%Y-%m-%dT%H:%M:%S%z", tmp) == 0)
            snprintf(str, sizeof(str), "Failed to get time");
        printf("%s\n\n", str);

        // Print timer status
        printf("T0 %4d (once per second)\n", timer0_count);
        printf("T1 %4d (5 times per sec)\n", timer1_count);
        printf("T2 %4d | %4d/s (target: %d)\n", timer2_count,
               timer2_count_per_second, TIMER_2_TARGET);
        printf("T3 %4d | %4d/s (target: %d)\n", timer3_count,
               timer3_count_per_second, TIMER_3_TARGET);
        printf("\n");

        printf("Press START to exit");

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
