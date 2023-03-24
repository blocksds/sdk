// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>
#include <string.h>

#include <nds.h>

// Test for flush:
//
// - Test buffer to 0.
// - Read test buffer to load it into cache.
// - Copy from source buffer to test buffer. Writes will go to the cache because
//   it has been loaded in the previous step. If not, it would have been written
//   without loading it.
// - Execute DC flush. Regions that are flushed will be updated in RAM, regions
//   that aren't flushed won't be updated in RAM.
// - Copy test buffer to destination buffer with DMA. DMA can't see the cache,
//   so it copies the actual contents in RAM.
//
// Test for invalidate:
//
// - Test buffer to 0.
// - Read test buffer to load it into cache.
// - Copy from source buffer to test buffer. Writes will go to the cache because
//   it has been loaded in the previous step. If not, it would have been written
//   without loading it.
// - Execute DC invalidate. Regions that are invalidated will never be updated
//   in RAM (unless they have been updated in RAM before the invalidate).
// - Copy test buffer to destination buffer without DMA. We want to see the
//   parts of the buffer that are still in the cache and haven't been
//   invalidated.
//
// The results screen should look like this:
//
//   No operation:
//     Ones:    256     0     0 | Note that the results here may vary a bit
//     Twoes:   256     0     0 | because the cache is still active, we are
//                              | just not affecting it directly.
//   Flush range:
//     Ones:    128   128     0
//     Twoes:   128     0   128
//
//   Invalidate range:
//     Ones:    128   128     0
//     Twoes:   128     0   128
//
//   Flush all:
//     Ones:      0   256     0
//     Twoes:     0     0   256
//
//   Invalidate all:
//     Ones:    256     0     0
//     Twoes:   256     0     0

#define BUFFER_SIZE (CACHE_LINE_SIZE * 8)

ALIGN(CACHE_LINE_SIZE) volatile uint8_t buffer_source_1[BUFFER_SIZE];
ALIGN(CACHE_LINE_SIZE) volatile uint8_t buffer_source_2[BUFFER_SIZE];
ALIGN(CACHE_LINE_SIZE) volatile uint8_t buffer_test[BUFFER_SIZE];
ALIGN(CACHE_LINE_SIZE) volatile uint8_t buffer_destination[BUFFER_SIZE];
volatile uint8_t helper;

void zero(void)
{
    memset((void *)buffer_test, 0, sizeof(buffer_test));
    memset((void *)buffer_destination, 0, sizeof(buffer_destination));
    DC_FlushAll();
}

void copy_1(void)
{
    for (int i = 0; i < sizeof(buffer_test); i += CACHE_LINE_SIZE)
        helper = buffer_test[i];

    memcpy((void *)buffer_test, (void *)buffer_source_1, sizeof(buffer_test));
}

void copy_2(void)
{
    for (int i = 0; i < sizeof(buffer_test); i += CACHE_LINE_SIZE)
        helper = buffer_test[i];

    memcpy((void *)buffer_test, (void *)buffer_source_2, sizeof(buffer_test));
}

void copy_dma(void)
{
    dmaCopy((void *)buffer_test, (void *)buffer_destination, sizeof(buffer_test));
}

void copy_no_dma(void)
{
    memcpy((void *)buffer_destination, (void *)buffer_test, sizeof(buffer_test));
}

void count(const char *title)
{
    int zeroes = 0;
    int ones = 0;
    int twoes = 0;

    for (int i = 0; i < sizeof(buffer_destination); i++)
    {
        uint8_t v = buffer_destination[i];
        if (v == 0)
            zeroes++;
        else if (v == 0x11)
            ones++;
        else if (v == 0x22)
            twoes++;
    }

    printf("%s %4d %4d %4d\n", title, zeroes, ones, twoes);
}

void flush_half(void)
{
    DC_FlushRange((void *)buffer_test, sizeof(buffer_test) / 2);
}

void invalidate_half(void)
{
    DC_InvalidateRange((void *)buffer_test, sizeof(buffer_test) / 2);
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    memset((void *)buffer_source_1, 0x11, sizeof(buffer_source_1));
    memset((void *)buffer_source_2, 0x22, sizeof(buffer_source_2));

    uint32_t ticks;

    printf("No operation:\n");
    DC_FlushAll();
    cpuStartTiming(0);
    {
        zero();
        copy_1();
        copy_dma();
        count("  Ones:  ");

        zero();
        copy_2();
        copy_dma();
        count("  Twoes: ");
    }
    ticks = cpuEndTiming();
    printf("                         t=%lu", ticks);

    printf("Flush range:\n");
    DC_FlushAll();
    cpuStartTiming(0);
    {
        zero();
        copy_1();
        flush_half();
        copy_dma();
        count("  Ones:  ");

        zero();
        copy_2();
        flush_half();
        copy_dma();
        count("  Twoes: ");
    }
    ticks = cpuEndTiming();
    printf("                         t=%lu", ticks);

    printf("Invalidate range:\n");
    DC_FlushAll();
    cpuStartTiming(0);
    {
        zero();
        copy_1();
        invalidate_half();
        copy_no_dma();
        count("  Ones:  ");

        zero();
        copy_2();
        invalidate_half();
        copy_no_dma();
        count("  Twoes: ");
    }
    ticks = cpuEndTiming();
    printf("                         t=%lu", ticks);

    printf("Flush all:\n");
    DC_FlushAll();
    cpuStartTiming(0);
    {
        zero();
        copy_1();
        DC_FlushAll();
        copy_dma();
        count("  Ones:  ");

        zero();
        copy_2();
        DC_FlushAll();
        copy_dma();
        count("  Twoes: ");
    }
    ticks = cpuEndTiming();
    printf("                         t=%lu", ticks);

    printf("Invalidate all:\n");
    DC_FlushAll();
    cpuStartTiming(0);
    {
        zero();
        copy_1();
        DC_InvalidateAll();
        copy_no_dma();
        count("  Ones:  ");

        zero();
        copy_2();
        DC_InvalidateAll();
        copy_no_dma();
        count("  Twoes: ");
    }
    ticks = cpuEndTiming();
    printf("                         t=%lu", ticks);

    printf("\n");
    printf("Press START to exit\n");

    fflush(stdout);

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
