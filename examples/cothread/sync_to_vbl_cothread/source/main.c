// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

// This example shows that the cooperative multithreading scheduler waits for an
// interrupt correctly. The counts by the interrupt handler and by the main loop
// should be the same.

#include <stdio.h>

#include <nds.h>
#include <nds/cothread.h>

int irq_frame_count;

void vbl_handler(void)
{
    irq_frame_count++;
}

int main(int argc, char **argv)
{
    irqSet(IRQ_VBLANK, vbl_handler);

    consoleDemoInit();

    printf("IRQ:\n");
    printf("Loop:\n");
    printf("\n");
    printf("\n");
    printf("Press START to exit to loader\n");

    int loop_count = 0;
    while (1)
    {
        loop_count++;
        cothread_yield_irq(IRQ_VBLANK);

        consoleSetCursor(NULL, 6, 0);
        printf("%5d", irq_frame_count);
        consoleSetCursor(NULL, 6, 1);
        printf("%5d", loop_count);

        scanKeys();

        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
