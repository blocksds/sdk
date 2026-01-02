// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <inttypes.h>
#include <stdio.h>

#include <nds.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    while (1)
    {
        swiWaitForVBlank();

        u32 value = getBatteryLevel();
        unsigned int battery_level = value & BATTERY_LEVEL_MASK;
        bool charger_connected = value & BATTERY_CHARGER_CONNECTED;

        consoleClear();
        printf("Current charge level: %u\n", battery_level);
        printf("Charger connected: %s\n", charger_connected ? "Yes" : "No");
        printf("\n");
        printf("Press START to exit");

        scanKeys();

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
