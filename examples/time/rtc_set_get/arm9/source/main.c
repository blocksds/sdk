// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

// This example shows how to set the time of the real time clock of the DS.

#include <stdio.h>
#include <time.h>

#include <nds.h>

volatile uint32_t ok = 0;

void FIFO_UserValue32Handler(u32 value, void *userdata)
{
    ok = value;
}

typedef enum {
    s_year, s_month, s_day, s_hour, s_min, s_sec
} selection_t;

int main(int argc, char **argv)
{
    consoleDemoInit();

    rtcTimeAndDate rtc_time_and_date;

    rtc_time_and_date.year = 2023 - 2000;
    rtc_time_and_date.month = 11;
    rtc_time_and_date.day = 17;
    rtc_time_and_date.hours = 21;
    rtc_time_and_date.minutes = 35;
    rtc_time_and_date.seconds = 12;

    selection_t sel = s_year;

    keysSetRepeat(20, 5);

    // Setup the FIFO handler
    fifoSetValue32Handler(FIFO_USER_01, FIFO_UserValue32Handler, 0);

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

        // Print time adjustment interface
        const int coord_x[6] = {
            3, 6, 9, 12, 15, 18
        };
        printf("New time:\n\n");
        printf("\x1b[%dC^\n", coord_x[sel]);
        printf("%02u-%02u-%02u %02u:%02u:%02u\n",
               rtc_time_and_date.year + 2000, rtc_time_and_date.month,
               rtc_time_and_date.day, rtc_time_and_date.hours,
               rtc_time_and_date.minutes, rtc_time_and_date.seconds);
        printf("\x1b[%dCv\n", coord_x[sel]);
        printf("\n");

        // Print general information
        printf("A: Set time+date\n");
        printf("B: Set time\n");
        printf("Pad: Adjust time\n");
        printf("\n");
        printf("Last write ok = %lu\n", ok);
        printf("\n");
        printf("Press START to exit");

        scanKeys();

        uint16_t keys = keysDownRepeat();

        if (keys & KEY_A)
        {
            fifoSendDatamsg(FIFO_USER_01, sizeof(rtc_time_and_date),
                            (void *)&rtc_time_and_date);
        }

        if (keys & KEY_B)
        {
            rtcTime rtc_time = {
                rtc_time_and_date.hours,
                rtc_time_and_date.minutes,
                rtc_time_and_date.seconds,
            };

            fifoSendDatamsg(FIFO_USER_02, sizeof(rtc_time), (void *)&rtc_time);
        }

        if (keys & KEY_LEFT)
        {
            if (sel > s_year)
                sel--;
        }

        if (keys & KEY_RIGHT)
        {
            if (sel < s_sec)
                sel++;
        }

        if (keys & KEY_DOWN)
        {
            switch (sel)
            {
                case s_year:
                    rtc_time_and_date.year--;
                    break;
                case s_month:
                    rtc_time_and_date.month--;
                    break;
                case s_day:
                    rtc_time_and_date.day--;
                    break;
                case s_hour:
                    rtc_time_and_date.hours--;
                    break;
                case s_min:
                    rtc_time_and_date.minutes--;
                    break;
                case s_sec:
                    rtc_time_and_date.seconds--;
                    break;
            }
        }

        if (keys & KEY_UP)
        {
            switch (sel)
            {
                case s_year:
                    rtc_time_and_date.year++;
                    break;
                case s_month:
                    rtc_time_and_date.month++;
                    break;
                case s_day:
                    rtc_time_and_date.day++;
                    break;
                case s_hour:
                    rtc_time_and_date.hours++;
                    break;
                case s_min:
                    rtc_time_and_date.minutes++;
                    break;
                case s_sec:
                    rtc_time_and_date.seconds++;
                    break;
            }
        }

        if (keys & KEY_START)
            break;
    }

    return 0;
}
