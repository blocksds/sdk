// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// This test shows how to hardcode the size of initialized data in DTCM.
//
// Normally, user variables are placed at the start of DTCM and they grow
// upwards. However, the stack is placed at the end of DTCM and grows downwards,
// so some programs may find it easy to corrupt the variables in DTCM due to
// stack overflows.
//
// It's possible to place DTCM user variables at the end of DTCM by setting
// __dtcm_data_size to a hardcoded value. The user stack is placed right below
// the user data, so it can overflow DTCM into main RAM. However, the user will
// only be able to allocate up to the size specified in __dtcm_data_size.
//
// In this test you can modify the size of the array "test_array" to see if the
// test builds or not. You can also edit the Makefile to enable or remove the
// value of __dtcm_data_size and see how that affects the allocation of the
// array. Another way to define this symbol is to define it in an assembly file.
// Check `dtcm_size.s` to see how to do it.

#include <stdio.h>

#include <nds.h>

// Addresses taken from mpu_setup.s in libnds
const uintptr_t dtcm_base = 0x02FF0000;
const uintptr_t dtcm_end = dtcm_base + (16 * 1024) - 1;

// This array has the exact size that we have allocated for DTCM data in the
// Makefile:
//
//     -Wl,--defsym,__dtcm_data_size=16
//
// If you increase the size to this array to, for example, 32, it will fail to
// build.
DTCM_DATA volatile uint8_t test_array[16] = { 0, 1, 2 };

int main(int argc, char **argv)
{
    consoleDemoInit();

    uintptr_t array_base = (uintptr_t)&test_array[0];

    printf("DSi mode: %d\n", isDSiMode());
    printf("\n");
    printf("Printing test_array: %u %u %u\n",
           test_array[0], test_array[1], test_array[2]);
    printf("\n");
    printf("DTCM Base: 0x%08X\n", dtcm_base);
    printf("Array:     0x%08X\n", array_base);
    printf("DTCM End:  0x%08X\n", dtcm_end);
    printf("\n");

    // The only symbol in DTCM should be the array defined in this file. If
    // __dtcm_data_size isn't set, it will be placed at the start of DTCM. If it
    // is set, it will be placed at the end of DTCM to let the stack overflow
    // DTCM and grow into main RAM.
    if (dtcm_base == array_base)
    {
        printf("__dtcm_data_size defined: NO\n");
        printf("\n");
    }
    else
    {
        printf("__dtcm_data_size defined: YES\n");
        printf("\n");
    }

    printf("\n");
    printf("Press START to exit\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint32_t keys_down = keysDown();
        if (keys_down & KEY_START)
            break;
    }

    return 0;
}
