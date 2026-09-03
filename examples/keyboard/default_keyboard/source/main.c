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
    KeyboardModifierMode mode = KeyboardModifiersIgnore; // Default

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        consoleClear();

        const char *mode_str[] = {
            // CTRL and ALT are treated like all other buttons. Default behaviour.
            [KeyboardModifiersIgnore]      = "Don't stick ",

            // CTRL and ALT are held until another key is pressed (not CTRL or ALT).
            [KeyboardModifiersStickOnce]   = "Stick once  ",

            // CTRL and ALT are held until they are pressed again.
            [KeyboardModifiersStickAlways] = "Stick always"
        };

        printf("START: Exit to loader\n");
        printf("L/R:   CTRL/ALT: %s\n", mode_str[mode]);
        printf("\n");
        printf("Space: %zu chars\n", sizeof(string) - 1);

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

        u16 keys_down = keysDown();

        if (keys_down & KEY_START)
            break;

        if (keys_down & KEY_L)
        {
            if (mode > KeyboardModifiersIgnore)
                mode--;
            keyboardModifierModeSet(mode);
        }
        else if (keys_down & KEY_R)
        {
            if (mode < KeyboardModifiersStickAlways)
                mode++;
            keyboardModifierModeSet(mode);
        }
    }

    return 0;
}
