// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    while (1)
    {
        swiWaitForVBlank();

        uint32_t value = getBatteryLevel();
        uint32_t battery_level = value & 0xF;
        uint32_t charger_connected = value & BIT(7);

        consoleClear();
        printf("Current charge level: %" PRIu32 "\n", battery_level);
        printf("Charger connected: %s\n", charger_connected ? "Yes" : "No");
        printf("\n");
        printf("Press START to exit");

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
