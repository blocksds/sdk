// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023-2025
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023

#include <nds.h>
#include <stdbool.h>
#include <stdio.h>

static bool rumble_toggle = false;
static uint8_t rumble_strength = 0;

void rumble_toggler(void)
{
    if (rumble_toggle)
        setRumble(rumble_strength);
    else
        setRumble(0);

    rumble_toggle = !rumble_toggle;
}

void example_set_rumble(int strength)
{
    if (strength == 0)
    {
        timerStop(0);
        setRumble(0);
        return;
    }

    if (peripheralSlot2GetSupportMask() & SLOT2_PERIPHERAL_RUMBLE_EDGE)
    {
        // If we're using an edge-triggered rumble cartridge we need to switch
        // between on and off at a high frequency to actually feel the
        // vibration. This starts a timer that will cause an interrupt 100 times
        // per second.
        rumble_strength = strength;
        timerStart(0, ClockDivider_256, timerFreqToTicks_256(100), rumble_toggler);
    }
    else
    {
        setRumble(strength);
    }
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    bool exit = false;

    while (!exit)
    {
        consoleClear();
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

        if (peripheralSlot2GetSupportMask() & SLOT2_PERIPHERAL_TILT)
            peripheralSlot2TiltStart();

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
                example_set_rumble(rumble);
            }

            // Go to the start of the line (it will stop at X=0)
            consoleAddToCursor(NULL, -32, 0);
            printf("\x1b[K"); // Clear line

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

                if (peripheralSlot2TiltRead(&pos))
                {
                    printf("Tilt: X=%03X, Y=%03X", pos.x, pos.y);
                    peripheralSlot2TiltStart();
                }
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
