// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <unordered_map>

#include <assert.h>

#include <nds.h>

#include "debug.h"
#include "vram_alloc.h"

// Address in VRAM where the tileset starts
static u8 *tileset_map_layer;

// Address in RAM where the tileset starts
static const u8 *tileset_data;

// We use a std::map here because we need a sparse array of tile_entry_t
// elements and it's easier to use the standard library than to implement it
// from scratch for this example.
typedef struct {
    u16 usage_count;
    u16 vram_tile_index;
} tile_entry_t;

std::unordered_map<int, tile_entry_t> tile_entry;

static tile_entry_t *tileset_find_tile_index(int index)
{
    // If the index isn't present in the std::map already this will allocate it
    tile_entry_t *t = &(tile_entry[index]);
    return t;
}

extern "C"
void tileset_reset(void)
{
    // Erase all elements in the map
    tile_entry.clear();

    vram_tiles_reset();

    if (debug_mode_is_enabled())
    {
        memset(tileset_map_layer, 0, 1024 * 8 * 8);
    }
}

extern "C"
void tileset_load(int map_layer, const void *data)
{
    tileset_reset();

    tileset_map_layer = (u8*)bgGetGfxPtr(map_layer);
    tileset_data = (const u8*)data;
}

extern "C"
int tileset_load_tile_index(int index)
{
    // This allocates the tile entry if it doesn't exist
    tile_entry_t *t = tileset_find_tile_index(index);

    if (t->usage_count == 0)
    {
        int vram_tile = vram_tile_find_free();
        assert(vram_tile != -1);

        t->vram_tile_index = vram_tile;
        vram_tile_set_used(vram_tile);

        const u8 *tile_addr = tileset_data + (8 * 8 * index);
        u8 *vram_addr = tileset_map_layer + (8 * 8 * vram_tile);

        memcpy(vram_addr, tile_addr, 8 * 8);
    }

    t->usage_count++;
    return t->vram_tile_index;
}

extern "C"
void tileset_unload_tile_index(int index)
{
    tile_entry_t *t = tileset_find_tile_index(index);

    assert(t->usage_count > 0);

    t->usage_count--;

    if (t->usage_count == 0)
    {
        vram_tile_set_free(t->vram_tile_index);

        if (debug_mode_is_enabled())
        {
            u8 *vram_addr = tileset_map_layer + (8 * 8 * t->vram_tile_index);
            memset(vram_addr, 0, 8 * 8);
        }

        tile_entry.erase(index);
    }
}
