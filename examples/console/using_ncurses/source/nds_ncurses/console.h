// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

#ifndef NDS_NCURSES_CONSOLE_H__
#define NDS_NCURSES_CONSOLE_H__

#include <nds/arm9/console.h>

#define CONSOLE_FONT_MIN        0
#define CONSOLE_FONT_MAX        4

#define CONSOLE_FONT_DEFAULT    2

extern PrintConsole main_console;
extern PrintConsole debug_console;

void console_get_character_dimension(int *width, int *height);
void console_setup(int font_index, int *columns, int *lines);

void debug_console_setup(void);

#endif // NDS_NCURSES_CONSOLE_H__
