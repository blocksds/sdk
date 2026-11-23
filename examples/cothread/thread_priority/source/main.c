// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

// Example of creating threads with different priorities.

#include <stdio.h>

#include <nds.h>
#include <nds/cothread.h>

#define NUM_THREADS (50)

static PrintConsole topScreen;
static PrintConsole bottomScreen;

comutex_t console_mutex;

volatile int thread_counter[NUM_THREADS];

int entrypoint_thread(void *arg)
{
    int index = (int)arg;

    int count = (1 + index) * 20000;

    while (count > 0)
    {
        thread_counter[index] = count;
        count--;
    }

    thread_counter[index] = -1;

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

    cothread_t threads[NUM_THREADS];

    cothread_t thread_id;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        // Give each group of 4 threads a different priority
        int8_t priority = i / 4;

        thread_id = cothread_create_priority(entrypoint_thread, (void *)i, 0,
                                             priority, COTHREAD_DETACHED);
        if (thread_id == -1)
        {
            printf("Failed to create thread %d.\n", i);
            while (1)
                swiWaitForVBlank();
        }
    }

    // The preemptive scheduler changes thread contexts whenever there is an
    // interrupt. However, if we only do that once per frame, with the VBL
    // interrupt, threads won't switch context very frequently. Instead,
    // setup a timer to cause IRQs (without an interrupt handler) so that the
    // scheduler switches between our threads frequently.
    cothread_setup_time_slicing(2, 2000); // 2000 IRQs per second (~33 per frame)

    // Make main() have higher priority than the other threads so that it can
    // refresh the screen in a fluid way.
    cothread_set_priority(0, 50);

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);

        bool any_thread_running = false;

        for (int i = 0; i < NUM_THREADS; i++)
        {
            if (threads[i] == -1)
                continue;

            int x = (i % 4) * 8;
            int y = (i / 4);

            if (thread_counter[i] == -1)
            {
                threads[i] = -1;

                consoleSelect(&bottomScreen);
                consoleSetCursor(NULL, x, y);
                printf("DONE   ");
                fflush(stdout);
            }
            else
            {
                consoleSelect(&bottomScreen);
                consoleSetCursor(NULL, x, y);
                printf("%5d ", thread_counter[i]);
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
