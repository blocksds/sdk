// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

__attribute__((__section__(".text._start"), __externally_visible__, __used__))
int _start(void *arg)
{
    int val = *(int *)arg;

    if (val <= 10)
    {
        // If the value is small enough, return it multiplied by 3
        *(int *)arg = val * 3;
        return 0;
    }
    else
    {
        // If the value is greater than 20, fail
        *(int *)arg = -1;
        return -1;
    }
}
