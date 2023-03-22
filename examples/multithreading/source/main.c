// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

// Example of cooperative multithreading with libnds.

#include <stdio.h>

#include <nds.h>
#include <nds/cothread.h>

#define NUM_CORO (50)

int entrypoint_thread(void *arg)
{
    int index = (int)arg;

    int x = (index % 4) * 8;
    int y = (index / 4);
    int count = (1 + index) * 50;

    while (count > 0)
    {
        printf("\x1b[%d;%d;H%5d", y, x, count);
        fflush(stdout);
        cothread_yield();
        count--;
    }

    return index;
}

int entrypoint_thread_detached(void *arg)
{
    int count = (int)arg;

    for (int i = 0; i < 1000; i++)
    {
        printf("\x1b[23;0;H%5d", count + i);
        fflush(stdout);
        cothread_yield();
    }

    printf("\x1b[23;0;HDone    ");
    fflush(stdout);

    return 0;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    int threads[NUM_CORO];

    for (int i = 0; i < NUM_CORO; i++)
        threads[i] = cothread_create(entrypoint_thread, (void *)i, 0, 0);

    cothread_create(entrypoint_thread_detached, (void *)10000, 0,
                    COTHREAD_DETACHED);

    while (1)
    {
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

                printf("\x1b[%d;%d;H%s", y, x, (ret == i) ? "OK   " : "FAIL ");
                fflush(stdout);
            }
            else
            {
                any_thread_running = true;
            }
        }

        if (any_thread_running == false)
            break;

        cothread_yield();
    }

    printf("\x1b[18;0HPress START to exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        cothread_yield();

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
