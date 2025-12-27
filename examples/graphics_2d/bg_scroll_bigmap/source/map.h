// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// The functions in this file let you load a map to a layer of the main 2D
// engine and scroll it. The map can be bigger than the limits of the hardware.

#ifndef EXAMPLE_MAP_H__
#define EXAMPLE_MAP_H__

#include <stdlib.h>

// Load a map and setup the requested background layer. This won't actually
// refresh the screen with data, you need to call map_set_position() after this.
void map_load(int bg_layer, int map_base, int tile_base,
              const void *map, size_t map_width, size_t map_height,
              const void *tiles, size_t tiles_size,
              const void *palette, size_t palette_size);

// Functions to get the current scroll
int map_get_scroll_x(void);
int map_get_scroll_y(void);

// This function refreshes the scroll state in libnds. You need to call
// bgUpdate() after this.
void map_refresh_scroll(void);

// Functions that get the maximum possible scroll (the minimum is 0).
int map_get_max_scroll_x(void);
int map_get_max_scroll_y(void);

// Jump to the specified coordinates and refresh the screen.
void map_set_position(int x, int y);

// Scroll the background by an arbitrary value. The value can be positive or
// negative, and it can be as big as needed.
void map_scroll_x(int delta);
void map_scroll_y(int delta);

#endif // EXAMPLE_MAP_H__
