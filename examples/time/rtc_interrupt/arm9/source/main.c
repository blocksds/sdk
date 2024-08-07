// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

// Using the RTC interrupt has been deprecated in libnds because it's not a
// reliable way to keep track of the time. It doesn't work on the 3DS in DS/DSi
// mode, and it doesn't work on most emulators.
//
// However, it may still be useful in the future to keep it working. This test
// has been added so that we can verify it doesn't completely break over time.

#include <stdio.h>
#include <time.h>

#include <nds.h>

int main(int argc, char **argv)
{
    // Setup sub screen for the text console
    consoleDemoInit();

    while (1)
    {
        // Synchronize game loop to the screen refresh
        swiWaitForVBlank();

        consoleClear();

        // Print current time
        char str[100];
        time_t t = time(NULL);
        struct tm *tmp = localtime(&t);
        if (strftime(str, sizeof(str), "%Y-%m-%dT%H:%M:%S%z", tmp) == 0)
            snprintf(str, sizeof(str), "Failed to get time");
        printf("%s\n", str);

        printf("\n"
               "\n"
               "\n"
               "\n"
               "The RTC interrupt should make\n"
               "the time change every second.\n"
               "\n"
               "The 3DS doesn't support the RTC\n"
               "interrupt, so it won't change.\n"
               "\n"
               "no$gba and melonDS support it\n"
               "but other emulators don't.\n"
               "\n"
               "\n"
               "\n"
               "\n");

        // Print some controls
        printf("START:  Exit to loader\n");

        scanKeys();

        uint16_t keys = keysHeld();

        if (keys & KEY_START)
            break;
    }

    return 0;
}
