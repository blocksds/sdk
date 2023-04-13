// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

// Note: This test is here to make sure that the stdin integration with picolibc
// still works, but it shouldn't be used to get keyboard input in a normal
// application.
//
// For example, when backspace is pressed, it stores a backspace character in
// the input string instead of deleting one character.
//
// Also, it blocks the rest of the program because it waits until the user has
// pressed return to exit.
//
// For a better way of using the keyboard of libnds, check the keyboard example
// in the examples folder.

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

    // Load demo keyboard
    Keyboard *kbd = keyboardDemoInit();
    kbd->OnKeyPressed = on_key_pressed;

    char string[256];
    string[0] = '\0';

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        uint16_t keys = keysDown();

        // Clear console
        printf("\x1b[2J");

        printf("\x1b[0;0HPress START to exit to loader");
        printf("\x1b[2;0HPress SELECT to input name");
        printf("\x1b[3;0HSpace: %zu chars", sizeof(string) - 1);

        printf("\x1b[5;0H[%s]\n", string);
        printf("Length: %d\n", strlen(string));

        if (keys & KEY_SELECT)
        {
            printf("\x1b[9;0HName: ");
            scanf("%s", string);
        }

        if (keys & KEY_START)
            break;
    }

    return 0;
}




