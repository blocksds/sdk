// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

// Callback called whenever the keyboard is pressed so that a character is
// printed on the screen.
void on_key_pressed(int key)
{
   if (key > 0)
      printf("%c", key);
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Initialize demo keyboard and load graphics to VRAM
    Keyboard *kbd = keyboardDemoInit();
    kbd->OnKeyPressed = on_key_pressed;

    char string[256];
    string[0] = '\0';

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysDown();

        consoleClear();

        printf("Press START to exit to loader\n");
        printf("\n");
        printf("Press SELECT to input name\n");
        printf("Space: %zu chars\n", sizeof(string) - 1);

        printf("[%s]\n", string);
        printf("Length: %d\n", strlen(string));

        if (keys & KEY_SELECT)
        {
            printf("\n");
            printf("\n");
            printf("\n");
            printf("Name: ");
            // This will show the keyboard, wait for the user to input a string,
            // and return when the user presses "return".
            scanf("%255s", string);
        }

        if (keys & KEY_START)
            break;
    }

    return 0;
}
