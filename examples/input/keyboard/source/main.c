// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023

#include <stdio.h>

#include <nds.h>

// Note: It is possible to use "scanf()" to get user input, but it doesn't
// behave correctly with special keys like backspace.

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Load demo keyboard
    keyboardDemoInit();

    // Load graphics to VRAM and display it
    keyboardShow();

    char string[50];
    string[0] = '\0';
    int string_cursor = 0;

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        // Clear console
        printf("\x1b[2J");

        printf("\x1b[0;0HPress START to exit to loader");
        printf("\x1b[2;0HKeyboard test. Space: %zu chars", sizeof(string) - 1);

        int16_t c = keyboardUpdate();
        if (c != -1)
        {
            if (c == '\b') // Backspace
            {
                if (string_cursor > 0)
                {
                    string_cursor--;
                    string[string_cursor] = '\0';
                }
            }
            else if (c >= 32)
            {
                // Make sure to leave an empty character for the nul terminator
                if (string_cursor < (sizeof(string) - 1))
                {
                    string[string_cursor] = c;
                    string_cursor++;
                    string[string_cursor] = '\0';
                }
            }
        }

        printf("\x1b[5;0H[%s]\n", string);
        printf("Length: %d\n", strlen(string));

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
