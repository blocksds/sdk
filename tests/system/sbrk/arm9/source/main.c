// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <inttypes.h>
#include <stdio.h>

#include <nds.h>

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

    // Setup ARM7 console
    // ------------------

    consoleSelect(&bottomScreen);

    consoleArm7Setup(&bottomScreen, 1024);

    // Start tests
    // -----------

    // Start tests on the ARM7, any value will do for this FIFO handler
    fifoSendValue32(FIFO_USER_01, 0);

    consoleSelect(&topScreen);

    extern void *fake_heap_end;
    extern void *fake_heap_start;

    printf("Is DSi? %s\n", isDSiMode() ? "Yes" : "No");
    printf("\n");
    unsigned int heap_start = (unsigned int)fake_heap_start;
    unsigned int heap_end = (unsigned int)fake_heap_end;
    printf("fake_heap_start: 0x%x\n", heap_start);
    printf("fake_heap_end:   0x%x\n", heap_end);
    printf("heap size:       %u B\n", heap_end - heap_start);
    printf("\n");

    printf("Tests\n");
    printf("\n");
    printf("malloc(1 KB):   0x%x\n", (unsigned int)malloc(1024));
    printf("malloc(512 KB): 0x%x\n", (unsigned int)malloc(512 * 1024));
    printf("malloc(1 MB):   0x%x\n", (unsigned int)malloc(1 * 1024 * 1024));
    printf("malloc(1 MB):   0x%x\n", (unsigned int)malloc(1 * 1024 * 1024));
    printf("malloc(1 MB):   0x%x\n", (unsigned int)malloc(1 * 1024 * 1024));
    printf("malloc(2 MB):   0x%x\n", (unsigned int)malloc(2 * 1024 * 1024));
    printf("malloc(4 MB):   0x%x\n", (unsigned int)malloc(4 * 1024 * 1024));
    printf("malloc(4 MB):   0x%x\n", (unsigned int)malloc(4 * 1024 * 1024));
    printf("malloc(8 MB):   0x%x\n", (unsigned int)malloc(8 * 1024 * 1024));
    printf("\n");

    printf("Press START to exit");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysHeld();
        if (keys & KEY_START)
            break;
    }

    return 0;
}
