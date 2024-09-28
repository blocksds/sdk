// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023, 2024

#include <nds.h>
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    bool exit = false;

    while (!exit)
    {
        consoleClear();
        printf("Motion example\n");
        printf("==============\n");
        printf("\n");

        motion_init();

        printf("Device: %s\n", motion_get_name(motion_get_type()));
        printf("     X: Calibrate accelerometer\n");
        printf("     Y: Calibrate gyroscope\n");
        printf("SELECT: Re-initialize device\n");
        printf(" START: Return to loader\n");
        printf("\n");

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

            if (keys_down & KEY_X)
            {
                // When laying down flat, gravity continues acting on the
                // accelerometer insite the cartridge. As such, the X/Y
                // axis measurement will be zero, but the Z axis measurement
                // will be equal to G.
                motion_set_offs_x();
                motion_set_offs_y();
                motion_set_sens_z(motion_read_z());
            }

            if (keys_down & KEY_Y)
            {
                motion_set_offs_gyro();
            }

            consoleSetCursor(NULL, 0, 8);

            printf("----- Raw -----\n");
            printf("Accelerometer X: %d    \n", motion_read_x());
            printf("Accelerometer Y: %d    \n", motion_read_y());
            printf("Accelerometer Z: %d    \n", motion_read_z());
            printf("    Gyroscope Z: %d    \n\n", motion_read_gyro());

            printf("-- Calibrated --\n");
            printf("X acceleration: %d mG   \n", motion_acceleration_x());
            printf("Y acceleration: %d mG   \n", motion_acceleration_y());
            printf("Z acceleration: %d mG   \n", motion_acceleration_z());
            printf("    Z rotation: %d deg   \n", motion_rotation());
        }

        motion_deinit();
    }

    return 0;
}
