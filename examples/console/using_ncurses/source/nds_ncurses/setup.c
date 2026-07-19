// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

#include <stdio.h>

#include <nds.h>

#include <filesystem.h>

#include "console.h"
#include "console_resize.h"
#include "input.h"

void nds_setup_input_output(void)
{
    defaultExceptionHandler();
#ifdef DEBUG
    consoleDebugInit(DebugDevice_NOCASH);
#endif

    // Initialize system timer for code that uses sleep() and usleep()
    systemCounterSetup();

    // Setup the debug console on the bottom screen
    debug_console_setup();

    // Initialize sound to handle keyboard beeps
    soundEnable();

    // Initialize NitroFS
    if (!nitroFSInit(NULL))
    {
        perror("nitroFSInit()\n");
        while (1)
            swiWaitForVBlank();
    }

    // Print instructions to the bottom screen
    consoleSelect(&debug_console);
    printf("L/R:    Change font\n");
    printf("SELECT: Swap screens\n");
    printf("START:  Exit to loader");

    // Initialize our custom keyboard
    keyboard_mouse_setup();

    // Setup the main console on the top screen
    int columns, lines;
    console_setup(CONSOLE_FONT_DEFAULT, &columns, &lines);

    // Select the top screen to use it from now on
    consoleSelect(&main_console);
}

// Setup environment variables. For now, we use the TERMCAP environment variable
// to describe the capabilities of the terminal.

static char *envs[] =
{
    // COLUMNS and LINES are required, but they are set manually with setenv()
    // in the console resize code.

    // Terminal name as specified in the terminfo file
    "TERM=libnds-256color",

    // Path to the directory with the terminfo files
    "TERMINFO=nitro:/terminfo",

#if 0
    // TERMCAP describes the capabilities of the terminal, but it's a much more
    // limited way of describing the capabilities of a terminal than terminfo.
    //
    // To enable support for this you need to build ncurses with option
    // --disable-database, which isn't used in the default build of BlocksDS.
    //
    // Note that TERMCAP can also point to a file instead of containing the
    // terminal description.
    //
    //   https://man.cat-v.org/unix_10th/5/termcap
    //   https://www.gnu.org/software/termutils/manual/termcap-1.3/html_mono/termcap.html
    //   https://www.man7.org/linux/man-pages/man5/termcap.5.html
    "TERMCAP=DS|libnds-256color|libnds console:" // Terminal name

    "co#42:li#24:"  // Number of columns and lines for the default font (6x8)

    "am:"           // The cursor wraps when it reaches the rightmost column
    "bs:"           // Backspace is supported
    "cm=\E[%d;%dH:" // Pattern to set cursor position
    "NP:"           // Pad character does not exist
    "NL:"           // Number of labels on screen (none)

    "cr=\r:"        // Carriage return
    "nw=\n:"        // New line

    "op=\E[39;49m:" // Reset foreground and background colors to their default values

    "E3=\E[2J:"     // How to clear the terminal's scrollback buffer.
    "U8#1:"         // Use UTF-8

    //"Co#8:pa#64"    // 8 colors, 64 pairs
    //"AF=\E[3%dm:"   // Set foreground color
    //"AB=\E[4%dm:"   // Set background color

    "Co#256:pa#1000:" // 256 colors, 1000 pairs
    "AF=\E[38;5;%dm:" // Set foreground color
    "AB=\E[48;5;%dm:" // Set background color

    "up=\E[A:" // Move up one line
    "do=\E[B:" // Move down one line
    "nd=\E[C:" // Move right one space
    "le=\E[D:" // Move left one space

    "cb=\E[1K:" // Clear to beginning of line
    "ce=\E[0K:" // Clear to end of line

    "cd=\E[0J:" // Clear to end of screen

    "bl=^G:" // Audible signal '\a'

    "ta=^I:" // '\t' Tab to next 8-space hardware tab stop

    "sf=\n:" // Scroll text up
    //"kl=^H:" // Delete-line key

    "ku=\E[A:" // Key up
    "kd=\E[B:" // Key down
    "kr=\E[C:" // Key right
    "kl=\E[D:" // Key left
    "kb=^H:"   // '\b' Backspace key

    "@8=\EOM:" // Enter/send key

    "md=\E[1m:" // Set bold mode
    "me=\E[0m:" // Turn off all attributes

    "BT#1:"     // Number of buttons on mouse
    "Km=\E[M:"  // Mouse event
    ,
#endif

    // End of environment variables
    NULL
};

char **environ = envs;
