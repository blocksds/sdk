// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023

#include <stdio.h>
#include <nds.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Initialize the Slot-2 peripheral.
    if (!peripheralSlot2Init(SLOT2_PERIPHERAL_ANY))
    {
        printf("Could not detect Slot-2 device!\n");
        goto exit;
    }

    printf("Device: %s\n", peripheralSlot2GetName());
    printf("Feature mask: %08lX\n", peripheralSlot2GetSupportMask());
    printf("RAM: %ld x %ld bytes\n", peripheralSlot2RamBanks(), peripheralSlot2RamSize());
    printf("     @ 0x%08X\n", (uintptr_t) peripheralSlot2RamStart());

exit:
    printf("\n");
    printf("Press START to exit to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint32_t keys_down = keysDown();
        if (keys_down & KEY_START)
            break;
    }

    return 0;
}
