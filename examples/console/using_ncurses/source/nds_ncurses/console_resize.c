// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

// libnds and libncurses have a few conflicts (WINDOW, KEY_* defines...). This
// file contains functions that need to use ncurses so that the other files that
// implement the NDS console and keyboard don't have conflicts.

#include <stdio.h>
#include <stdlib.h>

#include <ncurses.h>

#include "console.h"

void console_resize(int selected_font)
{
    int columns, lines;
    console_setup(selected_font, &columns, &lines);

    // Clear the screen and reset text attributes
    printf("\x1b[0m");
    printf("\x1b[2J");

    // Uncomment this if you want to clear the ncurses screen instead of keeping
    // its contents.
    //clear();

    char str[10];
    snprintf(str, sizeof(str), "%d", columns);
    setenv("COLUMNS", str, true);
    snprintf(str, sizeof(str), "%d", lines);
    setenv("LINES", str, true);

    resizeterm(lines, columns);

    // Tell ncurses to redraw the whole screen in the next refresh()
    redrawwin(stdscr);

    // Some debug output

    //wmove(stdscr, 0, 0);
    //wprintw(stdscr, "%dx%d\n", columns, lines);

    //int maxy, maxx;
    //getmaxyx(stdscr, maxy, maxx);
    //wprintw(stdscr, "%dx%d\n", maxx, maxy);

    //wprintw(stdscr, "%sx%s\n", getenv("COLUMNS"), getenv("LINES"));
    //wprintw(stdscr, "%dx%d\n", main_console.consoleWidth, main_console.consoleHeight);
    //wprintw(stdscr, "%dx%d\n", main_console.windowWidth, main_console.windowHeight);

    // Don't call refresh() from here, let the main loop handle it
    //refresh();

    // Send a bell character to the console to make a beep
    beep();
}
