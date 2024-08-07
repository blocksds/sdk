// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <nds.h>

// Returns the size of main RAM in bytes.
//
// DS retail: 4 MB.
// - Cached mirror at 0x02000000.
// - 3 uncached mirrors at 0x02400000, 0x02800000, 0x02C00000.
//
// DS debugger: 8 MB
// - Cached mirror at 0x02000000.
// - Uncached mirror at 0x02800000.
//
// DSi retail: 16 MB
// - Cached mirror at 0x02000000.
// - Uncached mirror at 0x0C000000.
//
// DSi debugger: 32 MB
// - Cached mirror of first 16 MB at 0x02000000.
// - Uncached mirror of first 16 MB at 0x0C000000.
// - Uncached second 16 MB at 0x0D000000.
size_t get_system_ram_size(void)
{
    if (isDSiMode())
    {
        if (isHwDebugger())
            return 32 * 1024 * 1024;
        else
            return 16 * 1024 * 1024;
    }
    else
    {
        if (isHwDebugger())
            return 8 * 1024 * 1024;
        else
            return 4 * 1024 * 1024;
    }
}

size_t region_size_to_bytes(uint32_t size)
{
    switch (size)
    {
        case CP15_REGION_SIZE_4KB:
            return 4 * 1024;
        case CP15_REGION_SIZE_8KB:
            return 8 * 1024;
        case CP15_REGION_SIZE_16KB:
            return 16 * 1024;
        case CP15_REGION_SIZE_32KB:
            return 32 * 1024;
        case CP15_REGION_SIZE_64KB:
            return 64 * 1024;
        case CP15_REGION_SIZE_128KB:
            return 128 * 1024;
        case CP15_REGION_SIZE_256KB:
            return 256 * 1024;
        case CP15_REGION_SIZE_512KB:
            return 512 * 1024;
        case CP15_REGION_SIZE_1MB:
            return 1 * 1024 * 1024;
        case CP15_REGION_SIZE_2MB:
            return 2 * 1024 * 1024;
        case CP15_REGION_SIZE_4MB:
            return 4 * 1024 * 1024;
        case CP15_REGION_SIZE_8MB:
            return 8 * 1024 * 1024;
        case CP15_REGION_SIZE_16MB:
            return 16 * 1024 * 1024;
        case CP15_REGION_SIZE_32MB:
            return 32 * 1024 * 1024;
        case CP15_REGION_SIZE_64MB:
            return 64 * 1024 * 1024;
        case CP15_REGION_SIZE_128MB:
            return 128 * 1024 * 1024;
        case CP15_REGION_SIZE_256MB:
            return 256 * 1024 * 1024;
        case CP15_REGION_SIZE_512MB:
            return 512 * 1024 * 1024;
        case CP15_REGION_SIZE_1GB:
            return 1024 * 1024 * 1024;
        // The next two are valid, but not used on NDS, and it won't fit in a
        // 32-bit int, so let's just ignore them.
        //case CP15_REGION_SIZE_2GB:
        //    return 2 * 1024 * 1024 * 1024;
        //case CP15_REGION_SIZE_4GB:
        //    return 4 * 1024 * 1024 * 1024;
        default:
            return 0;
    }
}

void print_human_size(size_t size)
{
    if (size < 1024)
    {
        printf("%3u B", size);
        return;
    }

    size /= 1024;
    if (size < 1024)
    {
        printf("%3u KB", size);
        return;
    }

    size /= 1024;
    if (size < 1024)
    {
        printf("%3u MB", size);
        return;
    }

    size /= 1024;
    printf("%3u GB", size);
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("DSi mode: %s\n", isDSiMode() ? "yes" : "no");
    printf("RAM size: %u MB\n", get_system_ram_size() / (1024 * 1024));
    printf("\n");

    printf("MPU regions:\n");
    printf("============\n");

    uint32_t regions[8] = {
        CP15_GetRegion0(), CP15_GetRegion1(), CP15_GetRegion2(), CP15_GetRegion3(),
        CP15_GetRegion4(), CP15_GetRegion5(), CP15_GetRegion6(), CP15_GetRegion7()
    };

    uint32_t data_cacheable = CP15_GetDataCachable();
    uint32_t instruction_cacheable = CP15_GetInstructionCachable();

    for (int i = 0; i < 8; i++)
    {
        uint32_t region = regions[i];

        bool enabled = region & CP15_CONFIG_REGION_ENABLE ? true : false;
        uintptr_t base = region & CP15_CONFIG_REGION_BASE_MASK;
        size_t size = region_size_to_bytes(region & CP15_CONFIG_REGION_SIZE_MASK);

        if (!enabled)
        {
            printf("%d. Disabled\n", i);
            continue;
        }

        bool dc = data_cacheable & CP15_CONFIG_AREA_IS_CACHABLE(i) ? true : false;
        bool ic = instruction_cacheable & CP15_CONFIG_AREA_IS_CACHABLE(i) ? true : false;

        printf("%d. 0x%08zx ", i, base);
        print_human_size(size);
        if ((base == 0x02000000) || (base == 0x02800000) || (base == 0x0C000000))
            printf(" RAM");
        else
            printf("    ");

        printf(" : %c %c", dc ? 'D' : ' ' , ic ? 'I' : ' ');
        printf("\n");
    }

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
