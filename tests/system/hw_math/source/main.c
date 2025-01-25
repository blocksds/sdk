// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <math.h>
#include <stdio.h>
#include <nds.h>
#define TEST_SIZE (1000)
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
volatile float vf;
volatile uint32_t vu;

uint32_t isqrt_asm(uint32_t n);

float hw_rsqrtf_asm(float x); //this is redundant, but makes testing this PR more convenient, can be removed later

ARM_CODE __attribute__ ((noinline)) uint32_t wiki_isqrt32(uint32_t x)
{
    //This is a modified version of the source in the wikipedia article on methods of computing square roots
    //https://en.m.wikipedia.org/wiki/Methods_of_computing_square_roots
    //which gives the original source as  
    //Square root by abacus algorithm, Martin Guy @ UKC, June 1985.
    //From a book on programming abaci by Mr C. Woo.
    //This particular version takes unsigned integers as input and uses the clz instruction instead of having two loops.
    uint32_t d=1<<(((31-__builtin_clz(x))>>1) <<1);
    uint32_t c = 0;
    while (d != 0) 
    {
        if ((int32_t)x >= c + d) 
        {      
            x -= c + d;        
            c = (c >> 1) + d;  
        }
        else 
        {
            c >>= 1;           
        }
        d >>= 2;               
    }
    return c;                  
}

ARM_CODE __attribute__ ((noinline)) uint32_t wiki_isqrt64(uint64_t n)
{
    //source is again wikipedia article on methods of computing square roots

    uint64_t x = n;

    // c_n
    uint64_t c = 0;

    // d_n which starts at the highest power of four <= n
    uint64_t d = ((uint64_t)1 )<< 62; // The second-to-top bit is set.
                         // Same as ((unsigned) INT32_MAX + 1) / 2.
    while (d > n) 
    {
        d >>= 2;
    }
    // for dₙ … d₀
    while (d != 0) 
    {
        if ((int64_t )x >= c + d) 
        {      // if X_(m+1) ≥ Y_m then a_m = 2^m
            x -= c + d;        // X_m = X_(m+1) - Y_m
            c = (c >> 1) + d;  // c_(m-1) = c_m/2 + d_m (a_m is 2^m)
        }
        else 
        {
            c >>= 1;           // c_(m-1) = c_m/2      (aₘ is 0)
        }
        d >>= 2;               // d_(m-1) = d_m/4
    }
    return c;                  // c_(-1)
}

static inline void slow_sqrt64(uint64_t a)
{
    // this starts a sqare root and also sets the mode beforehand
    // any writes to CNT and PARAM registers will restart the square root
    // so this will start one and then restart it when the second write happens
    REG_SQRTCNT = SQRT_64;
    REG_SQRT_PARAM = a;
}

static inline void fast_sqrt64(uint64_t a)
{
    //this writes the parameter and then immediately checks if it is already in the correct mode
    //for applications that do not change the mode often this can be 5-10% faster.
    REG_SQRT_PARAM = a;

    if ((REG_SQRTCNT & SQRT_MODE_MASK) != SQRT_64)
        REG_SQRTCNT = SQRT_64;
}

static inline void slow_sqrt32(uint32_t a)
{
    // this starts a sqare root and also sets the mode beforehand
    // any writes to CNT and PARAM registers will restart the square root
    // so this will start one and then restart it when the second write happens
    //which is not optimal
    REG_SQRTCNT = SQRT_32;
    REG_SQRT_PARAM_L = a;
}

static inline void fast_sqrt32(uint32_t a)
{
    //this writes the parameter and then immediately checks if it is already in the correct mode
    //for applications that do not change the mode often this can be 5-10% faster.
    REG_SQRT_PARAM_L = a;

    if ((REG_SQRTCNT & SQRT_MODE_MASK) != SQRT_32)
        REG_SQRTCNT = SQRT_32;
}

static inline int32_t sqrt_result(void)
{
    while (REG_SQRTCNT & SQRT_BUSY);

    return REG_SQRT_RESULT;
}

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
    }
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
    
    //Ensure that NaN returns the right NaN
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
    printf("\n");
    // Profile code
    // ------------
    {
        uint32_t sw_iterations = 0;
        uint32_t sw_time = 0;
        uint32_t hw_iterations = 0;
        uint32_t hw_time = 0;
        
        for (float f = (FLT_MIN * 1.1); f < (FLT_MAX * 0.9); f *= 1.01)
        {
            cpuStartTiming(0); // This uses timers 0 and 1
            sw_iterations++;
            // Write to a volatile register to prevent the compiler from
            // optimizing out the operation (it doesn't have any side-effect, so
            // the compiler would probably remove it without the write).
            vf = sqrtf(f);
            sw_time += cpuEndTiming();
        }
        
    
        
        for (float f = (FLT_MIN * 1.1); f < (FLT_MAX * 0.9); f *= 1.01)
        {
            hw_iterations++;
            cpuStartTiming(0);
            vf = hw_sqrtf(f);
            hw_time += cpuEndTiming();
        }
        
        printf("SW time: %lu (%lu per op)\n", sw_time, sw_time / sw_iterations);
        printf("HW time: %lu (%lu per op)\n", hw_time, hw_time / hw_iterations);
    
    }
    printf("SELECT: Test inverse SQRT\n");
    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysHeld() & KEY_SELECT)
            break;
    }

    {
        uint32_t asm_rsqrt_iterations = 0;
        uint32_t asm_rsqrt_time = 0;
        {
            for (float f = (FLT_MIN * 1.1); f < (FLT_MAX * 0.9); f *= 1.01)
            {
                asm_rsqrt_iterations++;
                cpuStartTiming(0);
                vf = hw_rsqrtf_asm(f);
                asm_rsqrt_time += cpuEndTiming();
            }
        }

        uint32_t naive_rsqrt_iterations = 0;
        uint32_t naive_rsqrt_time = 0;
        {
            for (float f = (FLT_MIN * 1.1); f < (FLT_MAX * 0.9); f *= 1.01)
            {
                naive_rsqrt_iterations++;
                cpuStartTiming(0);
                vf = 1.0f/hw_sqrtf(f);
                naive_rsqrt_time += cpuEndTiming();
            }
        }

        printf("asm time: %lu (%lu per op)\n", asm_rsqrt_time, asm_rsqrt_time / asm_rsqrt_iterations);
        printf("naive time: %lu (%lu per op)\n", naive_rsqrt_time, naive_rsqrt_time / naive_rsqrt_iterations);

    }

    {
        printf("Testing sqrt for correctness \n");
        uint64_t iterations = 0;
        for (float f = (FLT_MIN * 1.1); f < (FLT_MAX * 0.9); f *= 1.001)
        {
            iterations++;
            float sw = 1.0d/sqrt((double)f );
            float hw = hw_rsqrtf_asm(f);
            fint swf;
            fint hwf;
            swf.f=sw;
            hwf.f=hw;
            int32_t diff=(swf.u-hwf.u);
            if (diff<0) diff=-diff;

            if (sw != hw &&  diff>1)
            {
                printf("r sqrt(%e)\n", f);
                printf("  got:      %e\n", hw);
                printf("  expected: %e\n", sw);
                wait_forever();
            }
        }
        printf("Tested %llu iterations\n", iterations);
        printf("\n");
    }
    printf("SELECT: Brute force test all possible mantissas\n");
    printf("START: Skip test\n");
    uint32_t bruteforce=0;
    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysHeld() & KEY_SELECT)
            {
            bruteforce=1;
            break;
            }
        if (keysHeld() & KEY_START)
            break;

    }

    if (bruteforce)
    {
        printf("Testing rsqrt for correctness \n");
        printf("Please wait, this may take up to 5 min\n");
        uint64_t iterations = 0;
        uint32_t errors=0;
        for (fint mantissa ={.u= ( (126<<23)| 0 ) }; mantissa.u< ((128<<23) | 0) ; mantissa.u+=1)
        {
            iterations++;
            float sw = 1.0d/sqrt((double)mantissa.f );
            float hw = hw_rsqrtf_asm(mantissa.f);

            fint swf;
            fint hwf;
            swf.f=sw;
            hwf.f=hw;
            int32_t diff=(swf.u-hwf.u);
            if (diff<0) diff=-diff;

            if (diff!=0)errors++;

            if (diff>1)
            {
                printf("r sqrt(%e)\n", mantissa.f);
                printf("  got:      %e\n", hw);
                printf("  expected: %e\n", sw);
                wait_forever();
            }
        }
        printf("Tested %llu iterations\n", iterations);
        printf("%f percent correct", (1.0f-(float)errors/iterations )*100.0f);
        printf("\n");
    }
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

        for (uint32_t i = 0; i< TEST_SIZE; ++i)
        {
            sw_iterations++;
            // Write to a volatile register to prevent the compiler from
            // optimizing out the operation (it doesn't have any side-effect, so
            // the compiler would probably remove it without the write).
            vu = wiki_isqrt32(i<<16);
        }
        sw_time += cpuEndTiming();
        printf("wiki soft sqrt32 time: %lu (%lu per op)\n", sw_time, sw_time / sw_iterations);
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
            vu = isqrt_asm(i<<16);
        }
        sw_time += cpuEndTiming();
        printf("asm sqrt32 time: %lu (%lu per op)\n", sw_time, sw_time / sw_iterations);
    }

    printf("SELECT: Brute force test all possible 32-bit integer inputs\n");
    printf("This may take several hours. \n");
    printf("START: Skip test. A: Run test.\n");
    bruteforce=0;
    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysHeld() & KEY_A)
            {
            bruteforce=1;
            break;
            }
        if (keysHeld() & KEY_START)
            break;

    }

    if (bruteforce){
        printf("Testing asm sqrt correctness. \n");
        {
            uint64_t errors=0;
            for (uint64_t i = 0; i<  ((uint64_t)1u<<32  ); ++i)
            {
                // Write to a volatile register to prevent the compiler from
                // optimizing out the operation (it doesn't have any side-effect, so
                // the compiler would probably remove it without the write).
                vu = isqrt_asm(i);
                if (vu !=wiki_isqrt32(i))
                {
                    //printf("error : sqrt(%lu)= %lu ",i , vu);
                    errors+=1;
                }
            }
            printf("found %llu errors \n" , errors);
        }
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
            vu = sqrt_result();

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
            sqrt32_asynch(i);
            vu = sqrt32_result();
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
            vu = wiki_isqrt64( (((uint64_t )i)<<32)| i );
        }
        sw_time += cpuEndTiming();
        printf("soft sqrt64 time: %lu (%lu per op)\n", sw_time, sw_time / sw_iterations);
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
            vu = sqrt64_result();
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
            sqrt64_asynch( (((uint64_t )i)<<32)| i );
            vu = sqrt64_result();
        }
        sw_time += cpuEndTiming();
        printf("BlocksDS sqrt64 time: %lu (%lu per op)\n", sw_time, sw_time / sw_iterations);
    }
    printf("\n");
    // Done!
    wait_forever();
}
