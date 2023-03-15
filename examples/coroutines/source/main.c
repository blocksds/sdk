// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>
#include <ndsabi.h>

int entrypoint_coro(__ndsabi_coro_t *coro, void *arg)
{
    int index = (int)arg;

    int x = (index % 4) * 8;
    int y = (index / 4);
    int count = (1 + index) * 50;

    while (count > 0)
    {
        printf("\x1b[%d;%d;H%5d", y, x, count);
        fflush(stdout);
        __ndsabi_coro_yield(coro, 1);
        count--;
    }

    return 0;
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
            if (!cur->joined)
            {
                any_thread_running = true;
                __ndsabi_coro_resume(&(coro[i]));
            }
        }

        if (!any_thread_running)
            break;
    }

    printf("\x1b[18;0HPress START to exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
