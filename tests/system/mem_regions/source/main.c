// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

ITCM_CODE __attribute__((noinline))
int test_function(int a, int b)
{
    return a + b;
}

DTCM_DATA
int dtcm_data_var = 12345;

DTCM_BSS
int dtcm_bss_var = 0;

volatile int num1 = 2;
volatile int num2 = 3;

// Addresses taken from mpu_setup.s in libnds

const uintptr_t itcm_base = 0x01000000;
const uintptr_t itcm_end = itcm_base + (32 * 1024) - 1;

const uintptr_t dtcm_base = 0x02FF0000;
const uintptr_t dtcm_end = dtcm_base + (16 * 1024) - 1;

int main(int argc, char **argv)
{
    uintptr_t addr;

    consoleDemoInit();

    printf("ITCM\n");
    printf("----\n");
    printf("\n");

    addr = (uintptr_t)test_function;
    printf("Func result: %d (expect %d)\n", test_function(num1, num2), num1 + num2);
    printf("Func address: 0x%08X\n", addr);
    printf("In ITCM? %s\n", (addr >= itcm_base) && (addr <= itcm_end) ? "Yes" : "No");
    printf("\n");

    printf("DTCM\n");
    printf("----\n");
    printf("\n");

    addr = (uintptr_t)&dtcm_data_var;
    printf("DATA var: %d (expect %d)\n", dtcm_data_var, 12345);
    printf("Address: 0x%08X\n", addr);
    printf("In DTCM? %s\n", (addr >= dtcm_base) && (addr <= dtcm_end) ? "Yes" : "No");
    printf("\n");

    addr = (uintptr_t)&dtcm_bss_var;
    printf("BSS var: %d (expect %d)\n", dtcm_bss_var, 0);
    printf("Address: 0x%08X\n", addr);
    printf("In DTCM? %s\n", (addr >= dtcm_base) && (addr <= dtcm_end) ? "Yes" : "No");
    printf("\n");

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
