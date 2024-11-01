// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <math.h>
#include <stdio.h>

#include <nds.h>

#define TEST_SIZE (1000)

u32 dkp_sqrt32(int a)
{
    //This is the 32-bit integer sqrt used in devkitpro libnds commit 0834b49cbe4dc580ceff166926d1408f34fa059f
	REG_SQRTCNT = SQRT_32;

	while(REG_SQRTCNT & SQRT_BUSY);

	REG_SQRT_PARAM_L = a;

	while(REG_SQRTCNT & SQRT_BUSY);

	return REG_SQRT_RESULT;
}

u32 dkp_sqrt64(long long a)
{
    //This is the 64-bit integer sqrt used in devkitpro libnds commit 0834b49cbe4dc580ceff166926d1408f34fa059f
	REG_SQRTCNT = SQRT_64;

	while(REG_SQRTCNT & SQRT_BUSY);

	REG_SQRT_PARAM = a;

	while(REG_SQRTCNT & SQRT_BUSY);

	return REG_SQRT_RESULT;
}



static inline void slow_sqrt64(uint64_t a)
{
    REG_SQRTCNT = SQRT_64;
    REG_SQRT_PARAM = a;
}


static inline void fast_sqrt64(uint64_t a)
{
    REG_SQRT_PARAM = a;

    if ((REG_SQRTCNT & SQRT_MODE_MASK) != SQRT_64)
        REG_SQRTCNT = SQRT_64;
}


static inline void slow_sqrt32(uint32_t a)
{
    REG_SQRTCNT = SQRT_32;
    REG_SQRT_PARAM_L = a;
}



static inline void fast_sqrt32(uint32_t a)
{
    REG_SQRT_PARAM_L = a;

    if ((REG_SQRTCNT & SQRT_MODE_MASK) != SQRT_32)
        REG_SQRTCNT = SQRT_32;
}

static inline int32_t sqrt_result(void)
{
    while (REG_SQRTCNT & SQRT_BUSY);

    return REG_SQRT_RESULT;
}






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

    volatile float* vf=malloc(sizeof(float));
    if (vf==0) printf("could not allocate volatile float!\n");

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
            *vf = sqrtf(f);
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
            *vf = hw_sqrtf(f);
            hw_time += cpuEndTiming();
        }
    }

    printf("SW time: %lu (%lu per op)\n", sw_time, sw_time / sw_iterations);
    printf("HW time: %lu (%lu per op)\n", hw_time, hw_time / hw_iterations);



    printf("SELECT: Test integer SQRT\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysHeld() & KEY_SELECT)
            break;
    }



    {
            uint32_t sw_iterations = 0;
            uint32_t sw_time = 0;

            cpuStartTiming(0); // This uses timers 0 and 1

            for (int i = 0; i< TEST_SIZE; ++i)
            {
                sw_iterations++;
                // Write to a volatile register to prevent the compiler from
                // optimizing out the operation (it doesn't have any side-effect, so
                // the compiler would probably remove it without the write).
                TIMER3_CR = dkp_sqrt32(i);

            }

            sw_time += cpuEndTiming();
            printf("DKPsqrt32 time: %lu (%lu per op)\n", sw_time, sw_time / sw_iterations);

        }


    {
            uint32_t sw_iterations = 0;
            uint32_t sw_time = 0;

           cpuStartTiming(0); // This uses timers 0 and 1

           for (uint32_t i = 0; i< TEST_SIZE; ++i)
           {

                sw_iterations++;
                // Write to a volatile register to prevent the compiler from
                // optimizing out the operation (it doesn't have any side-effect, so
                // the compiler would probably remove it without the write).
                slow_sqrt32(i);
                TIMER3_CR = sqrt_result();

           }

           sw_time += cpuEndTiming();

           printf("slow_sqrt32 time: %lu (%lu per op)\n", sw_time, sw_time / sw_iterations);
    }


    {

            uint32_t sw_iterations = 0;
            uint32_t sw_time = 0;

           cpuStartTiming(0); // This uses timers 0 and 1

           for (uint32_t i = 0; i< TEST_SIZE; ++i)
           {
                sw_iterations++;
                // Write to a volatile register to prevent the compiler from
                // optimizing out the operation (it doesn't have any side-effect, so
                // the compiler would probably remove it without the write).
                fast_sqrt32(i);
                TIMER3_CR = sqrt_result();

           }

            sw_time += cpuEndTiming();

            printf("Fast sqrt32 time: %lu (%lu per op)\n", sw_time, sw_time / sw_iterations);
    }


    {

            uint32_t sw_iterations = 0;
            uint32_t sw_time = 0;

           cpuStartTiming(0); // This uses timers 0 and 1

           for (uint32_t i = 0; i< TEST_SIZE; ++i)
           {

                sw_iterations++;
                // Write to a volatile register to prevent the compiler from
                // optimizing out the operation (it doesn't have any side-effect, so
                // the compiler would probably remove it without the write).
                sqrt32_asynch(i);
                TIMER3_CR = sqrt32_result();

           }

            sw_time += cpuEndTiming();

            printf("BlocksDS sqrt32 time: %lu (%lu per op)\n", sw_time, sw_time / sw_iterations);
    }


    {
           uint32_t sw_iterations = 0;
           uint32_t sw_time = 0;

           cpuStartTiming(0); // This uses timers 0 and 1

           for (uint32_t i = 0; i< TEST_SIZE; ++i)
           {

                sw_iterations++;
                // Write to a volatile register to prevent the compiler from
                // optimizing out the operation (it doesn't have any side-effect, so
                // the compiler would probably remove it without the write).
                TIMER3_CR = dkp_sqrt64( (((uint64_t )i)<<32)| i );

           }

            sw_time += cpuEndTiming();

            printf("DKPsqrt64 time: %lu (%lu per op)\n", sw_time, sw_time / sw_iterations);
    }

    {
            uint32_t sw_iterations = 0;
            uint32_t sw_time = 0;

            cpuStartTiming(0); // This uses timers 0 and 1

            for (uint32_t i = 0; i< TEST_SIZE; ++i)
            {

                sw_iterations++;
                // Write to a volatile register to prevent the compiler from
                // optimizing out the operation (it doesn't have any side-effect, so
                // the compiler would probably remove it without the write).
                slow_sqrt64( (((uint64_t )i)<<32)| i );
                TIMER3_CR = sqrt64_result();


             }

            sw_time += cpuEndTiming();

            printf("slow_sqrt64 time: %lu (%lu per op)\n", sw_time, sw_time / sw_iterations);
    }
    {
            uint32_t sw_iterations = 0;
            uint32_t sw_time = 0;

            cpuStartTiming(0); // This uses timers 0 and 1

            for (uint32_t i = 0; i< TEST_SIZE; ++i)
            {

                sw_iterations++;
                // Write to a volatile register to prevent the compiler from
                // optimizing out the operation (it doesn't have any side-effect, so
                // the compiler would probably remove it without the write).
                fast_sqrt64( (((uint64_t )i)<<32)| i );
                TIMER3_CR = sqrt_result();


            }

            sw_time += cpuEndTiming();

            printf("Fast sqrt64 time: %lu (%lu per op)\n", sw_time, sw_time / sw_iterations);
    }




    {
            uint32_t sw_iterations = 0;
            uint32_t sw_time = 0;

            cpuStartTiming(0); // This uses timers 0 and 1

            for (uint32_t i = 0; i< TEST_SIZE; ++i)
            {

                sw_iterations++;
                // Write to a volatile register to prevent the compiler from
                // optimizing out the operation (it doesn't have any side-effect, so
                // the compiler would probably remove it without the write).
                sqrt64_asynch( (((uint64_t )i)<<32)| i );
                TIMER3_CR = sqrt64_result();


            }

            sw_time += cpuEndTiming();

            printf("BlocksDS sqrt64 time: %lu (%lu per op)\n", sw_time, sw_time / sw_iterations);
    }

    free((float*)vf);

    printf("\n");

    // Done!

    wait_forever();
}
