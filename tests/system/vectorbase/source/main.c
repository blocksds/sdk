// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Adrian "asie" Siekierka

#include <stdio.h>
#include <time.h>

#include <nds.h>

int timer0_count = 0;

void timer0_handler(void)
{
    timer0_count++;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("Using BIOS vectors     ");

    int oldIME = enterCriticalSection();
    setVectorBase(0xFFFF0000);
    leaveCriticalSection(oldIME);

    swiWaitForVBlank();
    timerStart(0, ClockDivider_256, timerFreqToTicks_256(10), timer0_handler);

    while (timer0_count < 10)
    {
        swiWaitForVBlank();
        printf(".");
    }

    printf(" OK\nUsing alternate vectors");

    // TODO: This only tests whether the BIOS vector mirrors at 0x00000000
    // work correctly. It would be good to have a custom IRQ handler to
    // verify that they're actually called from there.

    oldIME = enterCriticalSection();
    setVectorBase(0x00000000);
    leaveCriticalSection(oldIME);

    while (timer0_count < 20)
    {
        swiWaitForVBlank();
        printf(".");
    }

    printf(" OK\n\nPress START to exit");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysDown();
        if (keys & KEY_START)
            break;
    }

    return 0;
}
