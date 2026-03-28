// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

// This example checks that the MPU regions that allow code execution are the
// expected ones. The code is used by the exception handler installed with
// defaultExceptionHandler().

#include <stdio.h>

#include <nds.h>
#include <nds/arm9/exceptions.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("DSi mode: %d\n\n", isDSiMode());
    printf("Running tests...\n\n");

    struct
    {
        uintptr_t address;
        int region;
        bool is_code;
    }
    regions[] =
    {
         // Alternate vector base (disabled by default)
        { 0x0000000, 2, false },
        { 0x0000FFF, 2, false },
        { 0x0001000, -1, false },

        // ITCM
        { 0x0FFFFFF, -1, false },
        { 0x1000000, 4, true },
        { 0x1007FFF, 4, true },
        { 0x1008000, -1, false },

        // DS: Cacheable and non-cacheable main RAM
        // DSi: Cacheable main RAM
        { 0x1FFFFFF, -1, false },
        { 0x2000000, 6, true },
        { 0x23FFFFF, 6, true },
        { 0x2400000, isDSiMode() ? 6 : 5, true }, // Note: The tests will fail
        { 0x27FFFFF, isDSiMode() ? 6 : 5, true }, // in DS debugger units
        { 0x2800000, isDSiMode() ? 6 : 5, true },
        { 0x2FFFFFF, isDSiMode() ? 6 : 5, true },

        // DTCM
        { 0x2FF3FFF, isDSiMode() ? 6 : 5, true },
        { 0x2FF4000, 7, false },
        { 0x2FF7FFF, 7, false },
        { 0x2FF8000, isDSiMode() ? 6 : 5, true },

        // DSi switchable IWRAM
        { 0x3000000, isDSiMode() ? 3 : -1, isDSiMode() ? true : false },
        { 0x37FFFFF, isDSiMode() ? 3 : -1, isDSiMode() ? true : false },
        { 0x3800000, -1, false },

        // I/O registers and VRAM
        { 0x3FFFFFF, -1, false },
        { 0x4000000, 0, true },
        { 0x7FFFFFF, 0, true },

        // DS Accessory (GBA Cart)
        { 0x8000000, isDSiMode() ? -1 : 3, isDSiMode() ? false : true },

        // Non-cacheable main RAM (DSi)
        { 0xBFFFFFF, isDSiMode() ? -1 : 3, isDSiMode() ? false : true },
        { 0xC000000, isDSiMode() ? 5 : 3, true },
        { 0xCFFFFFF, isDSiMode() ? 5 : 3, true },
        // This will fail in DSi debuggers
        { 0xD000000, isDSiMode() ? -1 : 3, isDSiMode() ? false :true },
        { 0xDFFFFFF, isDSiMode() ? -1 : 3, isDSiMode() ? false : true },
        { 0xE000000, isDSiMode() ? -1 : 3, isDSiMode() ? false : true },
        { 0xEFFFFFF, isDSiMode() ? -1 : 3, isDSiMode() ? false : true },
        { 0xF000000, isDSiMode() ? -1 : 3, isDSiMode() ? false : true },
        { 0xFFFFFFF, isDSiMode() ? -1 : 3, isDSiMode() ? false : true },
        { 0x10000000, -1, false },

        // System ROM
        { 0xFFFEFFFF, -1, false },
        { 0xFFFF0000, 1, true },
        { 0xFFFFFFFF, 1, true },

        // End marker
        { 0xDEADDEAD, -1, false }
    };

    for (int i = 0; ; i++)
    {
        uintptr_t addr = regions[i].address;

        if (addr == 0xDEADDEAD)
            break;

        int reference_region = regions[i].region;
        bool reference_is_code = regions[i].is_code;

        int result_region = mpuRegionGet(addr);
        bool result_is_code = mpuRegionIsCode(result_region);

        if ((reference_is_code == result_is_code) && (reference_region == result_region))
            continue;

        printf("%08zX | %d,%d != expect %d,%d\n", addr, result_region, result_is_code,
               reference_region, reference_is_code);
    }

    printf("Press START to exit to loader");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
