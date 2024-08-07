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

    // Initialize demo keyboard and load graphics to VRAM
    keyboardDemoInit();

    // Show keyboard on the screen
    keyboardShow();

    char string[50];
    string[0] = '\0';
    int string_cursor = 0;

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        consoleClear();

        printf("Press START to exit to loader\n");
        printf("\n");
        printf("Keyboard test. Space: %zu chars\n", sizeof(string) - 1);

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

        printf("\n");
        printf("[%s]\n", string);
        printf("Length: %d\n", strlen(string));

        if (keysDown() & KEY_START)
            break;
    }

    return 0;
}
