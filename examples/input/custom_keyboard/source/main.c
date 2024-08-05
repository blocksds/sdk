// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <nds.h>

#include "keyboardGfx.h"

// Default keyboard map
static const s16 SimpleKbdLower[] =
{
    DVK_FOLD, DVK_FOLD, NOKEY, '1', '1', '2', '2', '3', '3', '4', '4', '5', '5',
    '6', '6', '7', '7', '8', '8', '9', '9', '0', '0', '-', '-', '=', '=',
    DVK_BACKSPACE, DVK_BACKSPACE, DVK_BACKSPACE, DVK_BACKSPACE, DVK_BACKSPACE,

    DVK_TAB, DVK_TAB, DVK_TAB, DVK_TAB, 'q', 'q', 'w', 'w', 'e', 'e', 'r', 'r',
    't', 't', 'y', 'y', 'u', 'u', 'i', 'i', 'o', 'o', 'p', 'p', '[', '[', ']',
    ']', '\\', '\\', '`', '`',

    DVK_CAPS, DVK_CAPS, DVK_CAPS, DVK_CAPS, DVK_CAPS, 'a', 'a', 's', 's', 'd',
    'd', 'f', 'f', 'g', 'g', 'h', 'h', 'j', 'j', 'k', 'k', 'l', 'l', ';', ';',
    '\'', '\'', DVK_ENTER, DVK_ENTER, DVK_ENTER, DVK_ENTER, DVK_ENTER,

    DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, 'z', 'z',
    'x', 'x', 'c', 'c', 'v', 'v', 'b', 'b', 'n', 'n', 'm', 'm', ',', ',', '.',
    '.', '/', '/', NOKEY, NOKEY, DVK_UP, DVK_UP, NOKEY, NOKEY,

    DVK_CTRL, DVK_CTRL, DVK_CTRL, DVK_CTRL, DVK_CTRL, DVK_ALT, DVK_ALT, DVK_ALT,
    DVK_ALT, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE,
    DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_MENU,
    DVK_MENU, DVK_MENU, DVK_MENU, DVK_MENU, DVK_LEFT, DVK_LEFT, DVK_DOWN,
    DVK_DOWN, DVK_RIGHT, DVK_RIGHT
};

static const s16 SimpleKbdUpper[] =
{
    DVK_FOLD, DVK_FOLD, NOKEY, '!', '!', '@', '@', '#', '#', '$', '$', '%', '%',
    '^', '^', '&', '&', '*', '*', '(', '(', ')', ')', '_', '_', '+', '+',
    DVK_BACKSPACE, DVK_BACKSPACE, DVK_BACKSPACE, DVK_BACKSPACE, DVK_BACKSPACE,

    DVK_TAB, DVK_TAB, DVK_TAB, DVK_TAB, 'Q', 'Q', 'W', 'W', 'E', 'E', 'R', 'R',
    'T', 'T', 'Y', 'Y', 'U', 'U', 'I', 'I', 'O', 'O', 'P', 'P', '{', '{', '}',
    '}', '|', '|', '~', '~',

    DVK_CAPS, DVK_CAPS, DVK_CAPS, DVK_CAPS, DVK_CAPS, 'A', 'A', 'S', 'S', 'D',
    'D', 'F', 'F', 'G', 'G', 'H', 'H', 'J', 'J', 'K', 'K', 'L', 'L', ':', ':',
    '"', '"', DVK_ENTER, DVK_ENTER, DVK_ENTER, DVK_ENTER, DVK_ENTER,

    DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, DVK_SHIFT, 'Z', 'Z',
    'X', 'X', 'C', 'C', 'V', 'V', 'B', 'B', 'N', 'N', 'M', 'M', '<', '<', '>',
    '>', '?', '?', NOKEY, NOKEY, DVK_UP, DVK_UP, NOKEY, NOKEY,

    DVK_CTRL, DVK_CTRL, DVK_CTRL, DVK_CTRL, DVK_CTRL, DVK_ALT, DVK_ALT, DVK_ALT,
    DVK_ALT, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE,
    DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_SPACE, DVK_MENU,
    DVK_MENU, DVK_MENU, DVK_MENU, DVK_MENU, DVK_LEFT, DVK_LEFT, DVK_DOWN,
    DVK_DOWN, DVK_RIGHT, DVK_RIGHT
};

static KeyMap capsLock =
{
    .mapDataPressed = keyboardGfxMap + 32 * 20,
    .mapDataReleased = keyboardGfxMap,
    .keymap = SimpleKbdUpper,
    .width = 32,
    .height = 5
};

static KeyMap lowerCase =
{
    .mapDataPressed = keyboardGfxMap + 32 * 30,
    .mapDataReleased = keyboardGfxMap + 32 * 10,
    .keymap = SimpleKbdLower,
    .width = 32,
    .height = 5
};

static Keyboard customKeyboard =
{
    .grid_width = 8,   // Grid width
    .grid_height = 16, // Grid height

    // By setting the initial state to uppercase, and marking the keyboard as
    // shifted, the first character will be uppercase and the next ones will be
    // lowercase, like with modern smartphone keyboards.
    .state = Upper,
    .shifted = true,

    .mappings = {
        &lowerCase, // keymap for lowercase
        &capsLock,  // keymap for caps lock
        0,          // keymap for numeric entry
        0           // keymap for reduced footprint
    },

    .tiles = keyboardGfxTiles,       // graphics tiles
    .tileLen = keyboardGfxTilesLen,  // graphics tiles length
    .palette = keyboardGfxPal,       // palette
    .paletteLen = keyboardGfxPalLen, // size of palette
    .tileOffset = 0,                 // tile offset
    .scrollSpeed = 3,                // scroll speed
    .OnKeyPressed = NULL,            // keypress callback
    .OnKeyReleased = NULL,           // key release callback
};

int main(int argc, char **argv)
{
    consoleDemoInit();

    // Load keyboard in the main screen, for a change
    videoSetMode(MODE_0_2D);

    // Move the main screen to the touch screen so that we can actually use the
    // keyboard.
    lcdMainOnBottom();

    // Load custom keyboard
    keyboardInit(&customKeyboard,
                 3,                // Background layer to use
                 BgType_Text4bpp,  // 16 color palette format
                 BgSize_T_256x512, // Background size
                 20,               // Map base
                 0,                // Tile base
                 true,             // Display it on the main screen
                 true);            // Load graphics to VRAM

    // Load graphics to VRAM and display it on the screen
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
        printf("\x1b[2;0HCustom keyboard test. Space: %zu chars",
               sizeof(string) - 1);

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
