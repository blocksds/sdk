// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

#include <ath6k/ath6k.h>

__attribute__((__section__(".text._start"), __externally_visible__, __used__))
int _start(void *arg)
{
    int val = *(int *)arg;

    if (val <= 10)
    {
        // If the value is small enough, return it multiplied by 3
        // Use a placeholder function of libath6k to test linking with the lib.
        *(int *)arg = xtensa_multiply(val, 3);
        //*(int *)arg = val * 3; // This is equivalent
        return 0;
    }
    else
    {
        // If the value is greater than 20, fail
        *(int *)arg = -1;
        return -1;
    }
}
