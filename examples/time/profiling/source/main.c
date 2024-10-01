// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <inttypes.h>
#include <math.h>
#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    float results[10];

    // Calculate sin() 10 times and see how long it took

    cpuStartTiming(0);

    for (int i = 0; i < 10; i++)
        results[i] = sin(0.1 * i);

    unsigned int count = cpuEndTiming();

    // Print results

    for (int i = 0; i < 10; i++)
        printf("%d: %f\n", i, results[i]);
    printf("\n");

    printf("Time: %u ticks (%" PRIu32 " us)\n", count, timerTicks2usec(count));
    printf("\n");

    printf("Press START to exit");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
