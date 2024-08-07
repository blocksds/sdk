// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>
#include <nds/cothread.h>

comutex_t mutex;
int count;
int total_produced, total_consumed;

int entrypoint_producer(void *arg)
{
    while (1)
    {
        comutex_acquire(&mutex);
        count++;
        total_produced++;
        comutex_release(&mutex);

        cothread_yield();
    }

    return 0;
}

int entrypoint_consumer(void *arg)
{
    while (1)
    {
        comutex_acquire(&mutex);
        if (count > 0)
        {
            count--;
            total_consumed++;
        }
        comutex_release(&mutex);

        cothread_yield();
    }

    return 0;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    cothread_create(entrypoint_consumer, NULL, 0, COTHREAD_DETACHED);
    cothread_create(entrypoint_producer, NULL, 0, COTHREAD_DETACHED);

    consoleSetCursor(NULL, 0, 18);
    printf("Press START to exit to loader\n");

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);

        consoleSetCursor(NULL, 0, 0);
        printf("Count:          %10d\n", count);
        printf("Total produced: %10d\n", total_produced);
        printf("Total consumed: %10d\n", total_consumed);

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
