// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// This test checks that the context of a thread is preserved after a
// cothread_yield() or cothread_yield_irq() call. It starts many threads and
// sets the values of registers r4-r11 to known values before yielding. After
// returning from the yield it checks that the values match the previous ones.
// This test requires assembly to write and read the values of the registers.

#include <inttypes.h>
#include <stdio.h>

#include <nds.h>

void set_registers_and_yield(uint32_t *arr1, uint32_t *arr2); // In test.s
void set_registers_and_yield_irq(uint32_t *arr1, uint32_t *arr2); // In test.s

void compare_arrays(const uint32_t *arr1, const uint32_t *arr2, void *arg)
{
    if (memcmp(arr1, arr2, sizeof(uint32_t) * 10) != 0)
    {
        printf("Failed! (%p)\n", arg);
        for (int i = 0; i < 10; i++)
            printf("%08" PRIx32 " | %08" PRIx32 "\n", arr1[i], arr2[i]);

        while (1)
            swiWaitForVBlank();
    }
}

int counter;

int run_test(void *arg)
{
    while (1)
    {
        printf("%d : Thread %p\n", counter++, arg);

        cothread_yield();

        uint32_t array_in[10];
        uint32_t array_out[10];

        for (int i = 0; i < 10; i++)
            array_in[i] = rand();

        set_registers_and_yield(&array_in[0], &array_out[0]);

        compare_arrays(&array_in[0], &array_out[0], arg);
    }

    return 0;
}

int run_test_irq(void *arg)
{
    while (1)
    {
        printf("%d : Thread IRQ %p\n", counter++, arg);

        cothread_yield_irq(IRQ_VBLANK);

        uint32_t array_in[10];
        uint32_t array_out[10];

        for (int i = 0; i < 10; i++)
            array_in[i] = rand();

        set_registers_and_yield_irq(&array_in[0], &array_out[0]);

        compare_arrays(&array_in[0], &array_out[0], arg);
    }

    return 0;
}

int main(int argc, char **argv)
{
    defaultExceptionHandler();

    PrintConsole topScreen;
    PrintConsole bottomScreen;

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    cothread_create(run_test, (void *)0, 0, COTHREAD_DETACHED);
    cothread_create(run_test, (void *)1, 0, COTHREAD_DETACHED);
    cothread_create(run_test, (void *)2, 0, COTHREAD_DETACHED);
    cothread_create(run_test_irq, (void *)3, 0, COTHREAD_DETACHED);
    cothread_create(run_test_irq, (void *)4, 0, COTHREAD_DETACHED);
    cothread_create(run_test_irq, (void *)5, 0, COTHREAD_DETACHED);
    cothread_create(run_test_irq, (void *)6, 0, COTHREAD_DETACHED);
    cothread_create(run_test_irq, (void *)7, 0, COTHREAD_DETACHED);
    cothread_create(run_test_irq, (void *)8, 0, COTHREAD_DETACHED);
    cothread_create(run_test_irq, (void *)9, 0, COTHREAD_DETACHED);
    cothread_create(run_test_irq, (void *)10, 0, COTHREAD_DETACHED);
    cothread_create(run_test_irq, (void *)11, 0, COTHREAD_DETACHED);

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);

        scanKeys();

        uint16_t keys = keysHeld();
        if (keys & KEY_START)
            break;

        printf("Main thread\n");

        run_test((void *)-1);
    }

    return 0;
}
