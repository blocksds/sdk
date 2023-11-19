// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023

#include <stdio.h>
#include <nds.h>

static void doReadTest(uint32_t bytes)
{
    cpuStartTiming(0);
    volatile uint16_t *ptr = peripheralSlot2RamStart();
    bool valid = true;
    for (int i = 0; i < bytes >> 1; i++)
    {
        if (ptr[i] != (i & 0xFFFF))
            valid = false;
    }
    uint64_t ticks = cpuEndTiming();

    printf("%llu ticks%s\n", ticks, valid ? "" : " ERR");
}

static void doWriteTest(uint32_t bytes)
{
    cpuStartTiming(0);
    volatile uint16_t *ptr = peripheralSlot2RamStart();
    for (int i = 0; i < bytes >> 1; i++)
    {
        ptr[i] = i;
    }
    uint64_t ticks = cpuEndTiming();

    printf("%llu ticks\n", ticks);
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("\x1b[2J"); // Clear console

    // Initialize the Slot-2 peripheral.
    if (!peripheralSlot2Init(SLOT2_PERIPHERAL_EXTRAM))
    {
        printf("Could not detect external RAM!\n");
        goto exit;
    }

    printf("Device: %s\n", peripheralSlot2GetName());
    printf("RAM: %ld x %ld bytes\n", peripheralSlot2RamBanks(), peripheralSlot2RamSize());
    printf("     @ 0x%08X\n", (uintptr_t) peripheralSlot2RamStart());

    // Open the Slot-2 peripheral - this is required.
    peripheralSlot2Open(SLOT2_PERIPHERAL_EXTRAM);

    uint32_t bytes_to_write = peripheralSlot2RamSize();

    printf("\nLarge write: ");
    doWriteTest(bytes_to_write);

    printf("Large read: ");
    doReadTest(bytes_to_write);

    for (int i = 0; i < 120; i++)
        swiWaitForVBlank();

    printf("-- 2K write: ");
    doWriteTest(2048);

    printf("-- 2K read: ");
    doReadTest(2048);

    printf("-- 2K read: ");
    doReadTest(2048);

    printf("-- 2K write: ");
    doWriteTest(2048);

    DC_FlushAll();
    peripheralSlot2EnableCache(false);

    printf("WT 2K write: ");
    doWriteTest(2048);

    printf("WT 2K read: ");
    doReadTest(2048);

    printf("WT 2K read: ");
    doReadTest(2048);

    printf("WT 2K read: ");
    doReadTest(2048);

    printf("WT 2K write: ");
    doWriteTest(2048);

    printf("WT 2K read: ");
    doReadTest(2048);

    DC_FlushAll();
    peripheralSlot2EnableCache(true);

    printf("WB 2K write: ");
    doWriteTest(2048);

    printf("WB 2K read: ");
    doReadTest(2048);

    printf("WB 2K read: ");
    doReadTest(2048);

    printf("WB 2K read: ");
    doReadTest(2048);

    printf("WB 2K write: ");
    doWriteTest(2048);

    printf("WB 2K read: ");
    doReadTest(2048);

    peripheralSlot2DisableCache();

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
