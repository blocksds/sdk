// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

// Example of preemptive multithreading with libnds.

#include <stdio.h>

#include <nds.h>
#include <nds/cothread.h>

#define NUM_CORO (50)

static PrintConsole topScreen;
static PrintConsole bottomScreen;

comutex_t console_mutex;

volatile int thread_counter[NUM_CORO];

int entrypoint_thread(void *arg)
{
    int index = (int)arg;

    int count = (1 + index) * 50000;

    while (count > 0)
    {
        thread_counter[index] = count;
        count--;
    }

    return index;
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

    cothread_t threads[NUM_CORO];

    cothread_t thread_id;

    for (int i = 0; i < NUM_CORO; i++)
    {
        thread_id = cothread_create(entrypoint_thread, (void *)i, 0, 0);
        if (thread_id == -1)
        {
            printf("Failed to create thread %d.\n", i);
            while (1)
                swiWaitForVBlank();
        }

        threads[i] = thread_id;

        int x = (i % 4) * 8;
        int y = (i / 4);

        consoleSelect(&topScreen);
        consoleSetCursor(NULL, x, y);

        printf("%X", (unsigned int)((cothread_info_t*)thread_id)->stack_top);
    }

    // The preemptive scheduler changes thread contexts whenever there is an
    // interrupt. However, if we only do that once per frame, with the VBL
    // interrupt, threads won't switch context very frequently. Instead,
    // setup a timer to cause IRQs (without an interrupt handler) so that the
    // scheduler switches between our threads frequently.

    TIMER_CR(2) = 0; // Stop it before setting it up
    TIMER_DATA(2) = TIMER_FREQ(2000); // 2000 IRQs per second (~33 per frame)
    irqEnable(IRQ_TIMER(2));
    TIMER_CR(2) = TIMER_IRQ_REQ | ClockDivider_1 | TIMER_ENABLE;

    // Make main() have higher priority than the other threads so that it can
    // refresh the screen in a fluid way.
    cothread_set_priority(0, 50);

    cothread_preemption_enable(true);

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);

        bool any_thread_running = false;

        for (int i = 0; i < NUM_CORO; i++)
        {
            if (threads[i] == -1)
                continue;

            int x = (i % 4) * 8;
            int y = (i / 4);

            if (cothread_has_joined(threads[i]))
            {
                int ret = cothread_get_exit_code(threads[i]);
                cothread_delete(threads[i]);
                threads[i] = -1;

                consoleSelect(&bottomScreen);
                consoleSetCursor(NULL, x, y);
                printf("%s", (ret == i) ? "OK     " : "FAIL   ");
                fflush(stdout);
            }
            else
            {
                consoleSelect(&bottomScreen);
                consoleSetCursor(NULL, x, y);
                printf("%5d", thread_counter[i]);
                fflush(stdout);

                any_thread_running = true;
            }
        }

        if (any_thread_running == false)
            break;
    }

    consoleSelect(&bottomScreen);
    consoleSetCursor(NULL, 0, 18);
    printf("Press START to exit to loader\n");

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
