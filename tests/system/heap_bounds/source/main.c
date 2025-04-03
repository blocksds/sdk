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
    PrintConsole topScreen;
    PrintConsole bottomScreen;

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    consoleSelect(&topScreen);

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

    size_t heap_grow_size = getHeapLimit() - getHeapEnd();
    printf("Heap grow size: %u B\n", heap_grow_size);

    // Run some tests

    consoleSelect(&bottomScreen);

    void *p;
    int ret;

    // This should succeed because we're allocating less space than available.
    p = malloc(heap_grow_size - (128 * 1024));
        printf("malloc(max - 128 KB): %p\n", p);
        printf("Heap end: %p\n", getHeapEnd());
    free(p);

    printf("\n");

    // Reserve some space. This should work because nothing is currently
    // allocated in the last 512 KB of RAM: sbrk() should have reduced the size
    // of the heap after the last free().
    printf("Heap end: %p\n", getHeapEnd());
    ret = reduceHeapSize(512 * 1024);
    printf("reduceHeapSize(512 KB): %d\n", ret);
    printf("Heap limit: %p\n", getHeapLimit());

    printf("\n");

    // This is the same malloc as before, but now it should fail because of the
    // reserved space. If this works, reduceHeapSize() didn't do its job.
    p = malloc(heap_grow_size - (128 * 1024));
        printf("malloc(max - 128 KB): %p\n", p);
        printf("Heap end: %p\n", getHeapEnd());
    free(p);

    printf("\n");

    // Allocate some memory, then try to reduce the available space for the heap
    // while that space is in use. The allocation should fail, but the reduction
    // shouldn't.
    p = malloc(heap_grow_size - (1 * 1024 * 1024));
        printf("malloc(max - 1 MB): %p\n", p);
        printf("Heap end: %p\n", getHeapEnd());
        ret = reduceHeapSize(2 * 1024 * 1024);
        printf("reduceHeapSize(2 MB): %d\n", ret);
        printf("Heap limit: %p\n", getHeapLimit());
    free(p);
    printf("free()\n");

    printf("\n");

    // After the free, the reduction should succeed.
    printf("Heap end: %p\n", getHeapEnd());
    ret = reduceHeapSize(2 * 1024 * 1024);
    printf("reduceHeapSize(2 MB): %d\n", ret);
    printf("Heap limit: %p\n", getHeapLimit());

    // End of tests

    consoleSelect(&topScreen);

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
