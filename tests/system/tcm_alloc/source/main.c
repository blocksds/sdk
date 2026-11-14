// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025-2026

// Test the DTCM and ITCM allocators and make sure that there is no corruption
// when all allocated buffers are used.

#include <stdio.h>

#include <nds.h>

u8 *getItcmHeapStart(void);
u8 *getItcmHeapLimit(void);
u8 *getDtcmHeapStart(void);
u8 *getDtcmHeapLimit(void);

static bool is_in_dtcm_heap(void *p, size_t size)
{
    uintptr_t addr = (uintptr_t)p;
    uintptr_t start = (uintptr_t)getDtcmHeapStart();
    uintptr_t end = (uintptr_t)getDtcmHeapLimit();

    if ((start <= addr) && ((addr + size) <= end))
        return true;

    return false;
}

static bool is_in_itcm_heap(void *p, size_t size)
{
    uintptr_t addr = (uintptr_t)p;
    uintptr_t start = (uintptr_t)getItcmHeapStart();
    uintptr_t end = (uintptr_t)getItcmHeapLimit();

    if ((start <= addr) && ((addr + size) <= end))
        return true;

    return false;
}

// Allocate some arrays and code in ITCM and DTCM statically

uint8_t DTCM_DATA_VAR(dtcm_test_data)[1 * 1024];
uint8_t DTCM_BSS_VAR(dtcm_test_bss)[1 * 1024];

uint8_t ITCM_DATA_VAR(itcm_test_data)[1 * 1024];
uint8_t ITCM_BSS_VAR(itcm_test_bss)[1 * 1024];

void ITCM_FUNC(initialize_arrays)(void)
{
    for (int i = 0; i < 1024; i++)
    {
        dtcm_test_data[i] = i;
        dtcm_test_bss[i] = -i;

        itcm_test_data[i] = i * 2;
        itcm_test_bss[i] = 65 - i;
    }
}

void ITCM_FUNC(check_arrays)(void)
{
    for (int i = 0; i < 1024; i++)
    {
        if (dtcm_test_data[i] != (uint8_t)i)
        {
            printf("dtcm_test_data[%i] failed\n", i);
            return;
        }

        if (dtcm_test_bss[i] != (uint8_t)-i)
        {
            printf("dtcm_test_bss[%i] failed\n", i);
            return;
        }

        if (itcm_test_data[i] != (uint8_t)(i * 2))
        {
            printf("itcm_test_data[%i] failed\n", i);
            return;
        }

        if (itcm_test_bss[i] != (uint8_t)(65 - i))
        {
            printf("itcm_test_bss[%i] failed\n", i);
            return;
        }
    }
}

void memset_check(void *buffer, int value, size_t size)
{
    uint8_t *p = buffer;

    for (size_t i = 0; i < size; i++)
    {
        if (p[i] != value)
        {
            printf("%s failed: %d != %d\n", __func__, p[i], value);
        }
    }
}

int main(int argc, char **argv)
{
    defaultExceptionHandler();

    PrintConsole topScreen;
    PrintConsole bottomScreen;

    // Redirect stderr to the no$gba debug console
    consoleDebugInit(DebugDevice_NOCASH);

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    consoleSelect(&topScreen);

    printf("DSi mode: %d\n", isDSiMode());
    printf("\n");

    printf("DTCM\n");
    printf("\n");
    printf("All:  0x%x - 0x%x\n", DTCM_START, DTCM_END);
    printf("Heap: %p - %p\n", getDtcmHeapStart(), getDtcmHeapLimit());
    printf("\n");
    printf("ITCM\n");
    printf("\n");
    printf("All:  0x%x - 0x%x\n", ITCM_START, ITCM_END);
    printf("Heap: %p - %p\n", getItcmHeapStart(), getItcmHeapLimit());

    consoleSelect(&bottomScreen);

    initialize_arrays();

    void *dtcm_p[8];
    void *itcm_p[8];

    for (int i = 0; i < 8; i++)
    {
        void *p = dtcm_malloc(2048);
        if (p == NULL)
            continue;

        if (!is_in_dtcm_heap(p, 2048))
        {
            printf("Pointer not in DTCM heap: %p\n", p);
            while (1)
                swiWaitForVBlank();
        }

        printf("DTCM: %p\n", p);
        memset(p, i, 2048);

        dtcm_p[i] = p;
    }

    for (int i = 0; i < 8; i++)
    {
        void *p = itcm_malloc(2048);
        if (p == NULL)
            continue;

        if (!is_in_itcm_heap(p, 2048))
        {
            printf("Pointer not in ITCM heap: %p\n", p);
            while (1)
                swiWaitForVBlank();
        }

        printf("ITCM: %p\n", p);
        memset(p, i, 2048);

        itcm_p[i] = p;
    }

    check_arrays();

    for (int i = 0; i < 8; i++)
    {
        if (itcm_p[i])
            memset_check(itcm_p[i], i, 2048);
        if (dtcm_p[i])
            memset_check(dtcm_p[i], i, 2048);

        itcm_free(itcm_p[i]);
        dtcm_free(dtcm_p[i]);
    }

    check_arrays();

    fprintf(stderr, "[TEST] Success\n");

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
