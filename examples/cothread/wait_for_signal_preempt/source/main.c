// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

// This example creates many threads. Each thread waits for a signal that is
// sent by the previous thread: 0 signals 1, 1 signals 2... and the last thread
// signals 0.
//
// expected_thread_index is used to verify that only the expected thread runs.

#include <stdio.h>

#include <nds.h>
#include <nds/cothread.h>

#define NUM_CORO (50)

int expected_thread_index = 0;

volatile int thread_counter[NUM_CORO];

int entrypoint_thread(void *arg)
{
    int index = (int)arg;

    // Index of the next thread in the chain
    int next_index = index + 1;
    if (next_index == NUM_CORO)
        next_index = 0;

    int count = 3000; // Number of iterations

    while (count > 0)
    {
        thread_counter[index] = count;

        cothread_yield_signal(index);

        if (index != expected_thread_index)
        {
            printf("Unexpected thread order! %d != %d", index, expected_thread_index);
            while (1)
                swiWaitForVBlank();
        }
        expected_thread_index = next_index;

        cothread_send_signal(next_index);

        count--;
    }

    return index;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Start threads

    cothread_t threads[NUM_CORO];

    for (int i = 0; i < NUM_CORO; i++)
    {
        cothread_t thread_id = cothread_create(entrypoint_thread, (void *)i, 0, 0);
        if (thread_id == -1)
        {
            printf("Failed to create thread %d.\n", i);
            while (1)
                swiWaitForVBlank();
        }

        threads[i] = thread_id;
    }

    // Give all threads the chance to setup and to wait for the initial signal
    cothread_yield_irq(IRQ_VBLANK);
    cothread_yield_irq(IRQ_VBLANK);

    TIMER_CR(2) = 0; // Stop it before setting it up
    TIMER_DATA(2) = TIMER_FREQ(2000); // 2000 IRQs per second (~33 per frame)
    irqEnable(IRQ_TIMER(2));
    TIMER_CR(2) = TIMER_IRQ_REQ | ClockDivider_1 | TIMER_ENABLE;

    cothread_preemption_enable(true);

    // Wake up thread 0
    cothread_send_signal(0);

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

                consoleSetCursor(NULL, x, y);
                printf("%s", (ret == i) ? "OK   " : "FAIL ");
                fflush(stdout);
            }
            else
            {
                consoleSetCursor(NULL, x, y);
                printf("%5d", thread_counter[i]);
                fflush(stdout);

                any_thread_running = true;
            }
        }

        if (any_thread_running == false)
            break;
    }

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
