// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <nds.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    consoleDemoInit();

    printf("Rumble example\n");
    printf("==============\n");
    printf("\n");

    rumbleInit();

    printf("Rumble detected: %s\n", isRumbleInserted() ? "yes" : "no");
    printf("Rumble type: %d\n", rumbleGetType());
    printf("\n");
    printf("A: Rumble ON\n");
    printf("B: Rumble OFF\n");
    printf("\n");
    printf("START: Return to loader\n");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys_down = keysDown();

        if (keys_down & KEY_B)
            setRumble(false);

        if (keys_down & KEY_A)
            setRumble(true);

        if (keys_down & KEY_START)
            break;
    }

    return 0;
}
