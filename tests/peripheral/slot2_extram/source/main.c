// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023

#include <stdio.h>
#include <nds.h>

// TODO: This currently only tests performance.
// It would be a good idea to make it validate the RAM, too.

static char ram_buffer[1048576];

static void doReadTest(uint32_t bytes)
{
    cpuStartTiming(0);
    char *ptr = (char*) peripheralSlot2RamStart();
    memcpy(ram_buffer, ptr, bytes);
    uint64_t ticks = cpuEndTiming();

    printf("%llu ticks\n", ticks);
}

static void doWriteTest(uint32_t bytes)
{
    cpuStartTiming(0);
    char *ptr = (char*) peripheralSlot2RamStart();
    memset(ptr, 0, bytes);
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
    if (bytes_to_write > sizeof(ram_buffer))
        bytes_to_write = sizeof(ram_buffer);

    printf("\nLarge read: ");
    doReadTest(bytes_to_write);

    printf("Large write: ");
    doWriteTest(bytes_to_write);

    printf("-- 2K write: ");
    doWriteTest(2048);

    printf("-- 2K read: ");
    doReadTest(2048);

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
