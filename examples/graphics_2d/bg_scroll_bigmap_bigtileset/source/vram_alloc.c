// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// This allocator needs to keep track of as many tiles as the screen can show.
// That is: ((256 / 8) + 2) * ((192 / 8) + 2) = 884. This is well below the 1024
// tile limit for regular maps, so there will never be a situation in which no
// more tiles can be loaded.

#include <string.h>

#include <nds.h>

#define MAX_USED_TILES (((SCREEN_WIDTH / 8) + 2) * ((SCREEN_HEIGHT / 8) + 2))

#define MAX_USED_TILES_ARRAY_SIZE ((MAX_USED_TILES / 32) + 1)
static u32 vram_used_tiles[MAX_USED_TILES_ARRAY_SIZE];

void vram_tiles_reset(void)
{
    memset(vram_used_tiles, 0, sizeof(vram_used_tiles));
}

void vram_tile_set_used(u32 index)
{
    // Leave tile index 0 empty
    index--;

    u32 entry = index / 32;
    u32 bit = index % 32;

    vram_used_tiles[entry] |= 1 << bit;
}

void vram_tile_set_free(u32 index)
{
    // Leave tile index 0 empty
    index--;

    u32 entry = index / 32;
    u32 bit = index % 32;

    vram_used_tiles[entry] &= ~(1 << bit);
}

int vram_tile_find_free(void)
{
    for (u32 entry = 0; entry < MAX_USED_TILES_ARRAY_SIZE; entry++)
    {
        for (u32 bit = 0; bit < 32; bit++)
        {
            if ((vram_used_tiles[entry] & (1 << bit)) == 0)
            {
                // Add 1 to leave tile index 0 empty
                return (entry * 32) + bit + 1;
            }
        }
    }

    return -1;
}

int vram_tiles_get_count_used(void)
{
    int count = 0;

    for (u32 entry = 0; entry < MAX_USED_TILES_ARRAY_SIZE; entry++)
    {
        for (u32 bit = 0; bit < 32; bit++)
        {
            if (vram_used_tiles[entry] & (1 << bit))
                count++;
        }
    }

    return count;
}

int vram_tiles_get_count_max(void)
{
    return MAX_USED_TILES;
}
