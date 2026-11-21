// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023

#include <stdio.h>
#include <nds.h>

static bool is_in_slot2_rom(void *p, size_t size)
{
    uintptr_t addr = (uintptr_t)p;
    uintptr_t start = (uintptr_t) 0x8000000;
    uintptr_t end = (uintptr_t) 0x9FFFFFF;

    if ((start <= addr) && ((addr + size) <= end))
        return true;

    return false;
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Redirect stderr to the no$gba debug console
    consoleDebugInit(DebugDevice_NOCASH);

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

    void *p = slot2_malloc(1024);
    if (!is_in_slot2_rom(p, 1024))
    {
        printf("malloc not in Slot-2 area: %p\n", p);
        goto exit;
    }
    p = slot2_realloc(p, 2048);
    if (!is_in_slot2_rom(p, 2048))
    {
        printf("realloc not in Slot-2 area: %p\n", p);
        goto exit;
    }
    slot2_free(p);

    printf("Test successful\n");
    fprintf(stderr, "[TEST] Success\n");

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
