// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

#include <nds.h>

#include "console.h"
#include "console_resize.h"
#include "input.h"

static bool keyboard_on_bottom = true;
static int selected_font = CONSOLE_FONT_DEFAULT;
static bool mouse_enabled = false;

static Keyboard *keyboard;

void mouse_enable(bool enable)
{
    mouse_enabled = enable;
}

static void keyboard_putc_to_ansi(int kc)
{
    if (kc == NOKEY)
        return;

    // Keys present in the keyboard that need to be ignored
    if ((kc == DVK_SHIFT) || (kc == DVK_CAPS) || (kc == DVK_CTRL) ||
        (kc == DVK_ALT) || (kc == DVK_MENU) || (kc == DVK_FOLD))
    {
        return;
    }

    // https://en.wikipedia.org/wiki/ANSI_escape_code#Control_Sequence_Introducer_commands
    if (kc == DVK_UP)
    {
        keyboardFifoPutc('\x1b');
        keyboardFifoPutc('[');
        keyboardFifoPutc('A');
    }
    else if (kc == DVK_DOWN)
    {
        keyboardFifoPutc('\x1b');
        keyboardFifoPutc('[');
        keyboardFifoPutc('B');
    }
    else if (kc == DVK_RIGHT)
    {
        keyboardFifoPutc('\x1b');
        keyboardFifoPutc('[');
        keyboardFifoPutc('C');
    }
    else if (kc == DVK_LEFT)
    {
        keyboardFifoPutc('\x1b');
        keyboardFifoPutc('[');
        keyboardFifoPutc('D');
    }
    // ???
    else if (kc == DVK_ENTER)
    {
        keyboardFifoPutc('\x1b');
        keyboardFifoPutc('O');
        keyboardFifoPutc('M');
    }
    // https://en.wikipedia.org/wiki/C0_and_C1_control_codes
    else if ((kc >= '@') && (kc <= '_')) // @, A-Z [ / ] ^ _
    {
        if (keyboard->altPressed)
        {
            keyboardFifoPutc('\x1b');
            keyboardFifoPutc(kc);
        }
        else if (keyboard->ctrlPressed)
        {
            keyboardFifoPutc(kc - '@');
        }
        else
        {
            keyboardFifoPutc(kc);
        }
    }
    else if ((kc >= 'a') && (kc <= 'z'))
    {
        if (keyboard->altPressed)
        {
            keyboardFifoPutc('\x1b');
            keyboardFifoPutc(kc);
        }
        else if (keyboard->ctrlPressed)
        {
            keyboardFifoPutc(kc - 'a' + 1);
        }
        else
        {
            keyboardFifoPutc(kc);
        }
    }
    // All other characters
    else
    {
        keyboardFifoPutc(kc);
    }
}

static void keypad_handle(void)
{
    bool refresh_font = false;

    uint16_t keys = keysDown();
    if (keys & KEY_L)
    {
        if (selected_font > CONSOLE_FONT_MIN)
        {
            selected_font--;
            refresh_font = true;
        }
    }
    else if (keys & KEY_R)
    {
        if (selected_font < CONSOLE_FONT_MAX)
        {
            selected_font++;
            refresh_font = true;
        }
    }

    if (refresh_font)
        console_resize(selected_font);

    if (keys & KEY_SELECT)
        keyboard_mouse_swap_screens();

    if (keys & KEY_START)
        exit(0);
}

static int keyboard_mouse_input_handle(void *arg)
{
    // Remember the coordinates pressed last time
    int last_x = 0;
    int last_y = 0;

    while (1)
    {
        scanKeys();

        keypad_handle();

        if (keyboard_on_bottom)
        {
            // Handle keyboard events

            keyboardFifoUpdate();
        }
        else
        {
            // Handle mouse events

            if (mouse_enabled)
            {
                // https://invisible-island.net/ncurses/man/curs_mouse.3x.html

                int x;
                int y;

                if (keysHeld() & KEY_TOUCH)
                {
                    touchPosition touch;
                    touchRead(&touch);

                    int w, h;
                    console_get_character_dimension(&w, &h);

                    x = (touch.px / w) + 1;
                    y = (touch.py / h) + 1;
                }
                else
                {
                    x = last_x;
                    y = last_y;
                }

                unsigned int button_event;
                bool send_event = false;

                if (keysDown() & KEY_TOUCH)
                {
                    // Press button 1: 1 - 1 = 0
                    button_event = 0;
                    send_event = true;

                    if (keyboard->ctrlPressed)
                        button_event |= 16; // Control
                    if (keyboard->altPressed)
                        button_event |= 8; // Meta
                    if (keyboard->shifted)
                        button_event |= 4; // Shift
                }
                else if (keysUp() & KEY_TOUCH)
                {
                    button_event = 3; // Release button
                    send_event = true;
                }
                else if (keysHeld() & KEY_TOUCH)
                {
                    // Only send updates if the cursor has moved
                    if ((x != last_x) || (y != last_y))
                    {
                        // Press button 1: 1 - 1 = 0
                        // Motion indicator: 32
                        button_event = 0 + 32;
                        send_event = true;
                    }
                }

                if (send_event)
                {
                    keyboardFifoPutc('\x1b');
                    keyboardFifoPutc('[');
                    keyboardFifoPutc('M');
                    keyboardFifoPutc(32 + button_event);
                    keyboardFifoPutc(32 + x);
                    keyboardFifoPutc(32 + y);
                }

                last_x = x;
                last_y = y;
            }
        }

        cothread_yield_irq(IRQ_VBLANK);
    }

    return 0;
}

void keyboard_mouse_setup(void)
{
    keyboard = keyboardDemoInit();
    keyboard->OnKeyPutc = keyboard_putc_to_ansi;
    keyboardModifierModeSet(KeyboardModifiersStickOnce);

    // Show keyboard on the screen
    keyboardShow();

    // Start a thread to handle keyboard and mouse input asynchronously
    cothread_create(keyboard_mouse_input_handle, NULL, 0, 0);
}

void keyboard_mouse_swap_screens(void)
{
    if (keyboard_on_bottom)
    {
        keyboard_on_bottom = false;
        lcdMainOnBottom();
    }
    else
    {
        keyboard_on_bottom = true;
        lcdMainOnTop();
    }
}
