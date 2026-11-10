// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

// Example of creating threads allocating the thread context and stack in
// different combinations of main RAM, DTCM and ITCM.

#include <stdio.h>

#include <nds.h>
#include <nds/cothread.h>

#define NUM_CORO (50)

static PrintConsole topScreen;
static PrintConsole bottomScreen;

int entrypoint_thread(void *arg)
{
    int index = (int)arg;

    int x = (index % 4) * 8;
    int y = (index / 4);

    register char *stack_ptr asm("sp");

    consoleSelect(&topScreen);
    consoleSetCursor(NULL, x, y);

    printf("%X", (unsigned int)stack_ptr);

    int count = (1 + index) * 50;

    while (count > 0)
    {
        consoleSelect(&bottomScreen);
        consoleSetCursor(NULL, x, y);
        printf("%5d", count);
        fflush(stdout);
        cothread_yield();
        count--;
    }

    return index;
}

int main(int argc, char **argv)
{
    defaultExceptionHandler();

    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&topScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottomScreen, 3,BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

    consoleSelect(&bottomScreen);
    consoleSelect(&topScreen);

    // Start threads

    cothread_t threads[NUM_CORO];

    cothread_t thread_id;

    for (int i = 0; i < NUM_CORO; i++)
    {
        // Different combinations of locations of the thread context and stack
        // to make sure that everything works.
        uint16_t settings[16] = {
            0, 0, 0, 0,

            COTHREAD_CONTEXT_IN_DTCM,
            COTHREAD_CONTEXT_IN_DTCM,
            COTHREAD_CONTEXT_IN_DTCM,

            COTHREAD_CONTEXT_IN_ITCM,
            COTHREAD_CONTEXT_IN_ITCM,
            COTHREAD_CONTEXT_IN_ITCM,

            COTHREAD_STACK_IN_DTCM,

            COTHREAD_STACK_IN_ITCM,

            COTHREAD_CONTEXT_IN_DTCM | COTHREAD_STACK_IN_DTCM,

            COTHREAD_CONTEXT_IN_ITCM | COTHREAD_STACK_IN_ITCM,

            COTHREAD_CONTEXT_IN_DTCM | COTHREAD_STACK_IN_ITCM,

            COTHREAD_CONTEXT_IN_ITCM | COTHREAD_STACK_IN_DTCM,
        };

        thread_id = cothread_create(entrypoint_thread, (void *)i, 0, settings[i % 16]);
        if (thread_id == -1)
        {
            printf("Failed to create thread %d.\n", i);
            while (1)
                swiWaitForVBlank();
        }

        threads[i] = thread_id;
    }

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

                consoleSelect(&bottomScreen);
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
