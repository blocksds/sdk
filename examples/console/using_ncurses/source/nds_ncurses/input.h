// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2026

#ifndef NDS_NCURSES_INPUT_H__
#define NDS_NCURSES_INPUT_H__

#include <stdbool.h>

void keyboard_mouse_setup(void);
void keyboard_mouse_swap_screens(void);
void mouse_enable(bool enable);

#endif // NDS_NCURSES_INPUT_H__
