// SPDX-License-Identifier: Zlib
//
// Copyright (C) 2026 Dominik Kurz

#include <float.h>
#include <math.h>
#include <stdio.h>

#include <nds.h>

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

void test(float y, float x)
{
    float a1 = atan2(y, x);
    float a2 = (float)atan2_f32(y * (1 << 16), x * (1 << 16)) / (1 << 12);
    float e = a1 - a2;

    int ulp = (int32_t)round(e * (1 << 12));
    if (ulp != 0)
    {
        printf("atan2f   atan2_f32 (%.6f, %.6f)", y, x);
        printf("%.6f %.6f\n", a1, a2);
        printf("error %e, %d ulp\n",e, ulp);
        wait_forever();
    }
}

void test_all(void)
{
    float y = 0;
    float x = 1.0;
    double diffmax = 0;
    float ulp = 0;

    while (y < (1 << 15))
    {
        float a1 = atan2(y,x);
        double a2 = (double)atan2_f32(y * (1 << 16), x * (1 << 16)) / (1 << 12);
        double diff = a1 - a2;
        diff = diff < 0 ? -diff : diff;

        if (diff > diffmax)
        {
            //printf("%f %f %e, a1=%f, a2=%f\n", y, x, diff, a1, a2);
            diffmax = diff;
            double t = (a1 * (1 << 12)) - (a2 * (1 << 12));
            t = t < 0 ? -t : t;
            ulp = t;
        }

        if (diff * (1 << 12) >= 0.7)
        {
            printf("y=%f, x=%f, a1=%f,a2=%f, diff=%f\n test failed, aborting\n",
                   y, x, a1, a2, diff);
            break;
        }

        float xn = (x - tan(0.01) * y);
        float yn = (tan(0.01) * x + y);
        x = xn;
        y = yn;
    }

    printf("max error: %e\n"
           "      ulp: %f\n\n", diffmax, ulp);

    if (ulp >= 1)
        wait_forever();
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("Testing individual cases...\n\n");

    test(0.5, 1);
    test(-0.5, 1);
    test(0.5, -1);
    test(-0.5, -1);
    test(1, 0.5);
    test(1, 1);
    test(1.0 / (1 << 12), 1.0 / (1 << 12));
    test(3.0, 1.0);
    test(1.0, 3.0);
    test(3.0, 0.0);
    test(0.0, 3.0);
    test(1.0, 0.000001);
    test(0x1.0p1, 0x1.0p-23);
    test(-1.0, 0.001);
    test(0.0001, 1.0);
    test(0.001, 1.0);

    int32_t y = (~0u) >> 1;
    int32_t x = y;
    float a1 = atan2(y, x);
    double a2 = (double)atan2_f32(y, x) / (1 << 12);
    double diff = a1 - a2;
    diff = diff < 0 ? -diff :diff;
    if (diff > 1)
        printf("%f, %f, %f\n", a1, a2, diff);

    printf("Testing all cases...\n\n");

    test_all();

    // Profile code
    // ------------

    uint32_t sw_iterations = 0;
    uint32_t sw_time = 0;
    {
        cpuStartTiming(0); // This uses timers 0 and 1
        for (float f = (FLT_MIN * 1.1); f < (1 << 15); f *= 1.01)
        {
            sw_iterations++;

            volatile float r = atan2f(f, 1.0f);
            (void)r;

        }
        sw_time += cpuEndTiming();
    }
    printf("atan2f  time:\n  (%lu per op)\n",  sw_time / sw_iterations);

    uint32_t hw_iterations = 0;
    uint32_t hw_time = 0;
    {
        cpuStartTiming(0);
        for (int32_t fx = 1; fx < (1 << 24); fx = fx + 1 + (fx >> 3))
        {
            hw_iterations++;

            volatile int32_t r = atan2_f32(fx, (1 << 12));
            (void)r;
        }
        hw_time += cpuEndTiming();
    }

    printf("atan2_f32 time:\n  (%lu per op)\n",  hw_time / hw_iterations);

    printf("\n");

    // Done!

    wait_forever();
}
