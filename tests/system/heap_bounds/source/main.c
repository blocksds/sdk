// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <stdio.h>

#include <nds.h>

// Addresses taken from mpu_setup.s in libnds

const uintptr_t dtcm_base = 0x02FF0000;
const uintptr_t dtcm_end = dtcm_base + (16 * 1024) - 1;

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("DSi mode: %d\n", isDSiMode());

    printf("\n");

    printf("Heap\n");
    printf("\n");
    printf("Base: %p\n", getHeapStart());
    printf("End:  %p\n", getHeapLimit());

    printf("\n");

    printf("DTCM\n");
    printf("\n");
    printf("Base: 0x%X\n", dtcm_base);
    printf("End:  0x%X\n", dtcm_end);

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
