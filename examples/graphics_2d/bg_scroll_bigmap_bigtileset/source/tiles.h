// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#ifndef EXAMPLE_TILES_H__
#define EXAMPLE_TILES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

// Reset internal state of the tile allocator
void tileset_reset(void);

// Initialize the tileset allocator
void tileset_load(int map_layer, const void *data);

// Load or unload a tile. They need to be called whenever a new tile is added
// to the map or removed from the map. They keep track of the usage of each
// tile. The tile index is the index of the original map, not the VRAM index.
int tileset_load_tile_index(int index);
void tileset_unload_tile_index(int index);

#ifdef __cplusplus
}
#endif

#endif // EXAMPLE_TILES_H__
