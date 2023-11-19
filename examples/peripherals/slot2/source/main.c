// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023

#include <nds.h>
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    bool exit = false;

    while (!exit)
    {
        printf("\x1b[2J"); // Clear console
        printf("Slot-2 example\n");
        printf("==============\n");
        printf("\n");

        peripheralSlot2InitDefault();

        printf("Device: %s\n", peripheralSlot2GetName());
        printf("Mask: %lb\n", peripheralSlot2GetSupportMask());
        printf("RAM: ");
        if (peripheralSlot2RamStart())
        {
            printf("yes, %ld x %ld bytes\n", peripheralSlot2RamBanks(), peripheralSlot2RamSize());
            printf("     @ 0x%08X\n", (uintptr_t) peripheralSlot2RamStart());
        }
        else
            printf("no\n");
        printf("\n");
        if (peripheralSlot2GetSupportMask() & SLOT2_PERIPHERAL_RUMBLE_ANY)
            printf("     A: Toggle rumble\n");
        printf("SELECT: Re-initialize Slot-2\n");
        printf(" START: Return to loader\n");
        printf("\n");

        uint8_t rumble = 0;

        while (1)
        {
            swiWaitForVBlank();

            scanKeys();

            uint16_t keys_down = keysDown();

            if (keys_down & KEY_SELECT)
            {
                exit = false;
                break;
            }

            if (keys_down & KEY_START)
            {
                exit = true;
                break;
            }

            if (keys_down & KEY_A)
            {
                rumble = (rumble + 1) % (rumbleGetMaxRawStrength() + 1);
                setRumble(rumble);
            }

            printf("\x1b[32D\x1b[K"); // Clear line

            if (peripheralSlot2GetSupportMask() & SLOT2_PERIPHERAL_GYRO_GPIO)
            {
                int result = peripheralSlot2GyroScan();
                if (result >= 0)
                    printf("Gyro: %04X", result);
            }

            if (peripheralSlot2GetSupportMask() & SLOT2_PERIPHERAL_SOLAR_GPIO)
            {
                int result = peripheralSlot2SolarScanFast();
                printf("Solar: %d", result);
            }

            if (peripheralSlot2GetSupportMask() & SLOT2_PERIPHERAL_TILT)
            {
                slot2TiltPosition pos;
                if (peripheralSlot2TiltUpdate(&pos))
                    printf("Tilt: X=%03X, Y=%03X", pos.x, pos.y);
            }

            if (peripheralSlot2GetSupportMask() & SLOT2_PERIPHERAL_GUITAR_GRIP)
            {
                guitarGripScanKeys();
                printf("Keys: %b", guitarGripKeysHeld());
            }

            if (peripheralSlot2GetSupportMask() & SLOT2_PERIPHERAL_PADDLE)
            {
                printf("Paddle: %03X", paddleRead());
            }

            if (peripheralSlot2GetSupportMask() & SLOT2_PERIPHERAL_PIANO)
            {
                pianoScanKeys();
                printf("Keys: %b", pianoKeysHeld());
            }
        }
    }

    return 0;
}
