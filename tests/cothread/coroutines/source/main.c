// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>
#include <ndsabi.h>

// In this example, this value is returned from the coroutine to mean that it
// hasn't finished yet. This isn't needed, as the "join" field of the context is
// set to 1 when the coroutine ends, but it is used to test that everything is
// working as expected.
#define COROUTINE_IN_PROGRESS -1

int entrypoint_coro(__ndsabi_coro_t *coro, void *arg)
{
    int index = (int)arg;

    int x = (index % 4) * 8;
    int y = (index / 4);
    int count = (1 + index) * 50;

    while (count > 0)
    {
        consoleSetCursor(NULL, x, y);
        printf("%5d", count);
        fflush(stdout);
        __ndsabi_coro_yield(coro, COROUTINE_IN_PROGRESS);
        count--;
    }

    return index;
}

#define NUM_CORO (50)

uint8_t ALIGN(4) stack[NUM_CORO][1024];
__ndsabi_coro_t coro[NUM_CORO];

int main(int argc, char **argv)
{
    consoleDemoInit();

    for (int i = 0; i < NUM_CORO; i++)
    {
        __ndsabi_coro_make(&(coro[i]),
            &(stack[i + 1]), // Pointer to the end
            entrypoint_coro,
            (void*)i);
    }

    while (1)
    {
        bool any_thread_running = false;

        for (int i = 0; i < NUM_CORO; i++)
        {
            __ndsabi_coro_t *cur = &(coro[i]);
            if (cur->joined)
                continue;

            any_thread_running = true;

            int x = (i % 4) * 8;
            int y = (i / 4);

            int ret = __ndsabi_coro_resume(&(coro[i]));
            if (cur->joined)
            {
                consoleSetCursor(NULL, x, y);
                printf("%s", (ret == i) ? "OK   " : "FAIL ");
                fflush(stdout);
            }
            else if (ret != COROUTINE_IN_PROGRESS)
            {
                // If this happens, there has been some error with yield(), stop
                // the coroutine.
                cur->joined = 1;

                consoleSetCursor(NULL, x, y);
                printf("%s", "ERROR");
                fflush(stdout);
            }
        }

        if (!any_thread_running)
            break;
    }

    consoleSetCursor(NULL, 0, 18);
    printf("Press START to exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
