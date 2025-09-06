// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023, 2025

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

int entrypoint_thread(void *arg)
{
    int index = (int)arg;

    // Index of the next thread in the chain
    int next_index = index + 1;
    if (next_index == NUM_CORO)
        next_index = 0;

    int x = (index % 4) * 8;
    int y = (index / 4);

    int count = 10; // Number of iterations

    while (count > 0)
    {
        consoleSetCursor(NULL, x, y);
        printf("%5d", count);
        fflush(stdout);

        cothread_yield_signal(index);

        if (index != expected_thread_index)
        {
            printf("Unexpected thread order!");
            while (1)
                swiWaitForVBlank();
        }
        expected_thread_index = next_index;

        // Wait for one frame before signaling the next thread so that we can
        // see the threads updating easily.
        cothread_yield_irq(IRQ_VBLANK);

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
        threads[i] = cothread_create(entrypoint_thread, (void *)i, 0, 0);

    // Give all threads the chance to setup and to wait for the initial signal
    cothread_yield_irq(IRQ_VBLANK);
    cothread_yield_irq(IRQ_VBLANK);

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

            if (cothread_has_joined(threads[i]))
            {
                int ret = cothread_get_exit_code(threads[i]);
                cothread_delete(threads[i]);
                threads[i] = -1;

                int x = (i % 4) * 8;
                int y = (i / 4);

                consoleSetCursor(NULL, x, y);
                printf("%s", (ret == i) ? "OK   " : "FAIL ");
                fflush(stdout);
            }
            else
            {
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
