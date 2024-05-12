// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <math.h>
#include <stdio.h>

#include <nds.h>

#define FLOAT_FORM(s, e, m)     (((s) << 31) | ((e) << 23) | ((m) << 0))

// This is the encoding used in ARM CPUs
const uint32_t signaling_nan = FLOAT_FORM(0, 0xFF, (0 << 22) | (1 << 0));
const uint32_t quiet_nan = FLOAT_FORM(0, 0xFF, (1 << 22) | (1 << 0));

const uint32_t positive_inf = FLOAT_FORM(0, 0xFF, 0);
const uint32_t negative_inf = FLOAT_FORM(1, 0xFF, 0);

typedef union {
    float f;
    uint32_t u;
} fint;

__attribute__((noreturn)) void wait_forever(void)
{
    printf("\n");
    printf("START: Exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysHeld() & KEY_START)
            exit(0);
    }
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Print some information

    printf("+inf: 0x%08lX\n", positive_inf);
    printf("-inf: 0x%08lX\n", negative_inf);
    printf("sNaN: 0x%08lX\n", signaling_nan);
    printf("qNaN: 0x%08lX\n", quiet_nan);
    printf("\n");

    // Test for correctness
    // --------------------

    {
        // This loop will test most of the range of values that a float can
        // hold. It will only test positive numbers, though, because hw_sqrtf()
        // doesn't check for negative numbers.
        uint64_t iterations = 0;
        for (float f = (FLT_MIN * 1.1); f < (FLT_MAX * 0.9); f *= 1.001)
        {
            iterations++;
            float sw = sqrtf(f);
            float hw = hw_sqrtf(f);
            if (sw != hw)
            {
                printf("sqrt(%e)\n", f);
                printf("  got:      %e\n", hw);
                printf("  expected: %e\n", sw);
                wait_forever();
            }
        }
        printf("Tested %llu iterations\n", iterations);
        printf("\n");

        // Test 1.0 and 0.0 as special cases

        {
            float val = 1.0;
            float sw = sqrtf(val);
            float hw = hw_sqrtf(val);
            if (sw != hw)
            {
                printf("sqrt(%e)\n", val);
                printf("  got:      %e\n", hw);
                printf("  expected: %e\n", sw);
                wait_forever();
            }
        }

        {
            float val = 0.0;
            float sw = sqrtf(val);
            float hw = hw_sqrtf(val);
            if (sw != hw)
            {
                printf("sqrt(%e)\n", val);
                printf("  got:      %e\n", hw);
                printf("  expected: %e\n", sw);
                wait_forever();
            }
        }

        // Ensure that NaN returns the right NaN

        {
            fint val;
            val.u = signaling_nan;

            fint sw, hw;
            sw.f = sqrtf(val.f);
            hw.f = hw_sqrtf(val.f);

            uint32_t sw_int = sw.u;
            uint32_t hw_int = hw.u;

            if (sw_int != hw_int)
            {
                printf("sqrt(0x%08lX)\n", val.u);
                printf("  got:           0x%08lX\n", hw_int);
                printf("  expected:      0x%08lX\n", sw_int);
                printf("\n");
            }

            val.u = quiet_nan;

            sw.f = sqrtf(val.f);
            hw.f = hw_sqrtf(val.f);

            sw_int = sw.u;
            hw_int = hw.u;

            if (sw_int != hw_int)
            {
                printf("sqrt(0x%08lX)\n", val.u);
                printf("  got:           0x%08lX\n", hw_int);
                printf("  expected:      0x%08lX\n", sw_int);
                printf("\n");
            }
        }

        // Ensure that negative numbers return the right NaN

        {
            float val = -1.0;
            fint sw, hw;
            sw.f = sqrtf(val);
            hw.f = hw_sqrtf(val);

            uint32_t sw_int = sw.u;
            uint32_t hw_int = hw.u;

            if (sw_int != hw_int)
            {
                printf("sqrt(%e)\n", val);
                printf("  got:           0x%08lX\n", hw_int);
                printf("  expected:      0x%08lX\n", sw_int);
                printf("\n");
            }
        }

        // Ensure that +inf returns +inf

        {
            fint val;
            val.u = positive_inf;

            fint sw, hw;
            sw.f = sqrtf(val.f);
            hw.f = hw_sqrtf(val.f);

            uint32_t sw_int = sw.u;
            uint32_t hw_int = hw.u;

            if (sw_int != hw_int)
            {
                printf("sqrt(0x%08lX)\n", val.u);
                printf("  got:      0x%08lX\n", hw_int);
                printf("  expected: 0x%08lX\n", sw_int);
                printf("\n");
            }
        }

        // Ensure that -inf returns NaN

        {
            fint val;
            val.u = negative_inf;

            fint sw, hw;
            sw.f = sqrtf(val.f);
            hw.f = hw_sqrtf(val.f);

            uint32_t sw_int = sw.u;
            uint32_t hw_int = hw.u;

            if (sw_int != hw_int)
            {
                printf("sqrt(0x%08lX)\n", val.u);
                printf("  got:      0x%08lX\n", hw_int);
                printf("  expected: 0x%08lX\n", sw_int);
                printf("\n");
            }
        }
    }
    printf("\n");

    // Profile code
    // ------------

    uint32_t sw_iterations = 0;
    uint32_t sw_time = 0;
    {
        for (float f = (FLT_MIN * 1.1); f < (FLT_MAX * 0.9); f *= 1.01)
        {
            cpuStartTiming(0); // This uses timers 0 and 1
            sw_iterations++;
            // Write to a volatile register to prevent the compiler from
            // optimizing out the operation (it doesn't have any side-effect, so
            // the compiler would probably remove it without the write).
            TIMER3_CR = sqrtf(f);
            sw_time += cpuEndTiming();
        }
    }

    uint32_t hw_iterations = 0;
    uint32_t hw_time = 0;
    {
        for (float f = (FLT_MIN * 1.1); f < (FLT_MAX * 0.9); f *= 1.01)
        {
            hw_iterations++;
            cpuStartTiming(0);
            TIMER3_CR = hw_sqrtf(f);
            hw_time += cpuEndTiming();
        }
    }

    printf("SW time: %lu (%lu per op)\n", sw_time, sw_time / sw_iterations);
    printf("HW time: %lu (%lu per op)\n", hw_time, hw_time / hw_iterations);

    printf("\n");

    // Done!

    wait_forever();
}
