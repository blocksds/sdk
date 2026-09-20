// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

// This test shows the size and location of DTCM and ITCM.

#include <stdio.h>

#include <nds.h>

size_t tcm_size_to_bytes(uint32_t size)
{
    switch (size)
    {
        case CP15_TCM_SIZE_4KB:
            return 4 * 1024;
        case CP15_TCM_SIZE_8KB:
            return 8 * 1024;
        case CP15_TCM_SIZE_16KB:
            return 16 * 1024;
        case CP15_TCM_SIZE_32KB:
            return 32 * 1024;
        case CP15_TCM_SIZE_64KB:
            return 64 * 1024;
        case CP15_TCM_SIZE_128KB:
            return 128 * 1024;
        case CP15_TCM_SIZE_256KB:
            return 256 * 1024;
        case CP15_TCM_SIZE_512KB:
            return 512 * 1024;
        case CP15_TCM_SIZE_1MB:
            return 1 * 1024 * 1024;
        default:
            return 0;
    }
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("DSi mode: %s\n", isDSiMode() ? "yes" : "no");
    printf("\n");

    uint32_t tcm_size_info = CP15_GetTCMSize();

    assert((tcm_size_info & CP15_TCM_DATA_RAM_ABSENT) == 0);
    assert((tcm_size_info & CP15_TCM_INSTRUCTION_RAM_ABSENT) == 0);

    int dtcm_size_id = (tcm_size_info & CP15_TCM_DATA_RAM_SIZE_MASK)
                        >> CP15_TCM_DATA_RAM_SIZE_SHIFT;
    size_t dtcm_size = tcm_size_to_bytes(dtcm_size_id);

    int itcm_size_id = (tcm_size_info & CP15_TCM_INSTRUCTION_RAM_SIZE_MASK)
                        >> CP15_TCM_INSTRUCTION_RAM_SIZE_SHIFT;
    size_t itcm_size = tcm_size_to_bytes(itcm_size_id);

    printf("DTCM\n");
    printf("====\n");
    printf("\n");

    printf("Size %d: %zu bytes\n", dtcm_size_id, dtcm_size);

    uintptr_t dtcm_base = CP15_GetDTCM() & CP15_TCM_BASE_MASK;
    uintptr_t dtcm_end = dtcm_base + dtcm_size;

    printf("0x%08X - 0x%08X\n", dtcm_base, dtcm_end);

    printf("\n");

    printf("ITCM\n");
    printf("====\n");
    printf("\n");

    printf("Size %d: %zu bytes\n", itcm_size_id, itcm_size);

    uintptr_t itcm_base = CP15_GetITCM() & CP15_TCM_BASE_MASK;
    uintptr_t itcm_end = itcm_base + itcm_size;

    printf("0x%08X - 0x%08X\n", itcm_base, itcm_end);

    consoleSetCursor(NULL, 0, 23);
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
