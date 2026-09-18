// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

// This example shows that the vertical blanking interrupt happens once per
// frame, and that the counter in the main loop increases at the same rate as
// the interrupt gets called.

#include <stdio.h>

#include <nds.h>

int irq_frame_count;

void vbl_handler(void)
{
    irq_frame_count++;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("IRQ:\n");
    printf("Loop:\n");
    printf("\n");
    printf("Diff:\n");
    printf("\n");
    printf("\n");
    printf("Press START to exit to loader\n");

    // Synchronize the code before the test to get consistent results every run
    swiWaitForVBlank();

    irqSet(IRQ_VBLANK, vbl_handler);

    int loop_count = 0;
    while (1)
    {
        swiWaitForVBlank();
        loop_count++;

        consoleSetCursor(NULL, 6, 0);
        printf("%5d", irq_frame_count);
        consoleSetCursor(NULL, 6, 1);
        printf("%5d", loop_count);
        consoleSetCursor(NULL, 6, 3);
        printf("%5d", loop_count - irq_frame_count); // Should be 0

        scanKeys();

        if (keysHeld() & KEY_START)
            break;
    }

    return 0;
}
