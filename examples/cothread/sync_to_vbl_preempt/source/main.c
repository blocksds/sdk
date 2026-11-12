// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026


#include <stdio.h>

#include <nds.h>
#include <nds/cothread.h>

int irq_frame_count;
int thread_frame_count;
int main_loop_frame_count;

void vbl_irq_handler(void)
{
    irq_frame_count++;
}

int vbl_thread(void *arg)
{
    (void)arg;

    while (1)
    {
        cothread_yield_irq(IRQ_VBLANK);
        thread_frame_count++;
    }
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    consoleDebugInit(DebugDevice_NOCASH);

    printf("IRQ:\n");
    printf("Thread:\n");
    printf("Loop:\n");
    printf("\n");
    printf("Diff:\n");
    printf("\n");
    printf("\n");
    printf("Press START to exit to loader\n");

    irqSet(IRQ_VBLANK, vbl_irq_handler);

    cothread_create(vbl_thread, NULL, 0, COTHREAD_DETACHED);

    TIMER_CR(2) = 0; // Stop it before setting it up
    TIMER_DATA(2) = TIMER_FREQ(1200); // 1200 IRQs per second (20 per frame)
    irqEnable(IRQ_TIMER(2));
    TIMER_CR(2) = TIMER_IRQ_REQ | ClockDivider_1 | TIMER_ENABLE;

    cothread_preemption_enable(true);

    // Synchronize all values now
    irq_frame_count = 0;
    thread_frame_count = 0;
    main_loop_frame_count = 0;

    while (1)
    {
        main_loop_frame_count++;

        cothread_yield_irq(IRQ_VBLANK);

        consoleSetCursor(NULL, 6, 0);
        printf("%5d", irq_frame_count);
        consoleSetCursor(NULL, 6, 1);
        printf("%5d", thread_frame_count);
        consoleSetCursor(NULL, 6, 2);
        printf("%5d", main_loop_frame_count);

        consoleSetCursor(NULL, 6, 4);
        int diff1 = main_loop_frame_count - irq_frame_count;
        int diff2 = main_loop_frame_count - thread_frame_count;
        printf("%d, %d", diff1, diff2); // Should be 0

        scanKeys();

        if (keysHeld() & KEY_START)
            break;

        if (main_loop_frame_count == 250)
        {
            if ((diff1 < 4) && (diff2 < 4))
                fprintf(stderr, "[TEST] Test passed\n");
            else
                fprintf(stderr, "[TEST] Test failed\n");
        }
    }

    return 0;
}
