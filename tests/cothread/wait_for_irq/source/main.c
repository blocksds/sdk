// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// Test of whether threads work as expected a of cooperative multithreading with libnds and of how to delete
// threads, etc.

#include <stdio.h>

#include <nds.h>
#include <nds/cothread.h>

// Defined in libnds
extern uint32_t cothread_threads_count;
extern uint32_t cothread_threads_wait_irq_count;

static PrintConsole topScreen;
static PrintConsole bottomScreen;

int thread_wait_for_irq(void *arg)
{
    unsigned int interrupt = (int)arg;

    consoleSelect(&bottomScreen);
    printf("Waiting for: 0x%X\n", interrupt);
    fflush(stdout);
    cothread_yield_irq(interrupt);

    return interrupt;
}

int thread_end(void *arg)
{
    unsigned int iterations = (int)arg;

    consoleSelect(&bottomScreen);
    printf("Looping for: %u\n", iterations);
    fflush(stdout);
    for (unsigned int i = 0; i < iterations; i++)
        cothread_yield();

    consoleSelect(&bottomScreen);
    printf("Looped for: %u\n", iterations);
    fflush(stdout);

    return 0;
}

int thread_forever(void *arg)
{
    consoleSelect(&bottomScreen);
    printf("Looping forever\n");
    fflush(stdout);
    while (1)
        cothread_yield();
}

void timer2_handler(void)
{
    timerStop(2);
}

void timer3_handler(void)
{
    timerStop(3);
}

int main(int argc, char **argv)
{
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    consoleSelect(&bottomScreen);

    // Start threads

    cothread_t th_timer0 = cothread_create(thread_wait_for_irq, (void *)IRQ_TIMER0, 0, 0);
    cothread_t th_timer1 = cothread_create(thread_wait_for_irq, (void *)IRQ_TIMER1, 0, COTHREAD_DETACHED);
    cothread_t th_timer2 = cothread_create(thread_wait_for_irq, (void *)IRQ_TIMER2, 0, 0);
    cothread_t th_timer3 = cothread_create(thread_wait_for_irq, (void *)IRQ_TIMER3, 0, COTHREAD_DETACHED);

    cothread_t th_forever = cothread_create(thread_forever, NULL, 0, 0);

    cothread_t th_0 = cothread_create(thread_end, (void *)200000, 0, 0);
    cothread_t th_1 = cothread_create(thread_end, (void *)100000, 0, COTHREAD_DETACHED);
    cothread_t th_2 = cothread_create(thread_end, (void *)1000000, 0, 0);
    cothread_t th_3 = cothread_create(thread_end, (void *)2000000, 0, COTHREAD_DETACHED);

    consoleSelect(&topScreen);
    printf("Press START to exit to loader\n");

    int count = 0;

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);

        consoleSelect(&topScreen);

        consoleSetCursor(NULL, 0, 2);
        printf("Frames: %d\nTotal %lu - Waiting %lu  \n", count++,
               cothread_threads_count, cothread_threads_wait_irq_count);

        scanKeys();

        if (keysDown() & KEY_A)
        {
            cothread_delete(th_timer0);
            cothread_delete(th_timer1);
        }

        if (keysDown() & KEY_B)
        {
            cothread_delete(th_timer2);
            cothread_delete(th_timer3);
        }

        if (keysDown() & KEY_X)
        {
            cothread_delete(th_forever);
        }

        if (keysDown() & KEY_Y)
        {
            timerStart(2, ClockDivider_1024, timerFreqToTicks_1024(10), timer2_handler);
            timerStart(3, ClockDivider_1024, timerFreqToTicks_1024(10), timer3_handler);
        }

        if (keysDown() & KEY_RIGHT)
        {
            cothread_delete(th_0);
            cothread_delete(th_1);
        }
        if (keysDown() & KEY_LEFT)
        {
            cothread_delete(th_2);
            cothread_delete(th_3);
        }

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
