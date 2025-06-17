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

        scanKeys();

        uint16_t keys_down = keysDown();
        uint16_t keys_held = keysHeld();
        uint16_t keys_up = keysUp();
        uint16_t keys_down_repeat = keysDownRepeat();

        consoleClear();

        printf("Press START+SELECT to exit\n\n");

        if ((keys_held & (KEY_START | KEY_SELECT)) == (KEY_START | KEY_SELECT))
            break;

        printf("Key input test\n\n");

        const char *key_names[15] = {
            "A", "B", "Select", "Start", "Right", "Left", "Up", "Down", "R",
            "L", "X", "Y", "Touch", "Lid", "Debug"
        };

        printf("Down: ");
        for (int i = 0; i <= 14; i++)
        {
            if (keys_down & BIT(i))
                printf("%s ", key_names[i]);
        }

        printf("\n\nHeld: ");
        for (int i = 0; i <= 14; i++)
        {
            if (keys_held & BIT(i))
                printf("%s ", key_names[i]);
        }

        printf("\n\nUp: ");
        for (int i = 0; i <= 14; i++)
        {
            if (keys_up & BIT(i))
                printf("%s ", key_names[i]);
        }

        printf("\n\nRepeat: ");
        for (int i = 0; i <= 14; i++)
        {
            if (keys_down_repeat & BIT(i))
                printf("%s ", key_names[i]);
        }
    }

    return 0;
}
