// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// We need to use a 512x256 map. When we scroll horizontally we modify one
// column of tiles. If the background is as wide as the screen (256 pixels) we
// will always see that column as we modify it. The next valid size for
// backgrounds is 512x256, so that's what we need to use. Vertical scroll isn't
// a problem because the screen is smaller than the background (192 pixels).

#include <nds.h>

// Size of the background in pixels
static int map_size_width;
static int map_size_height;

// Pointer to map data
static const void *map_data;

// Map ID returned by libnds
static int map_bg;

// Total scroll of the map
static int map_current_scroll_x;
static int map_current_scroll_y;

void map_load(int bg_layer, int map_base, int tile_base,
              const void *map, size_t map_width, size_t map_height,
              const void *tiles, size_t tiles_size,
              const void *palette, size_t palette_size)
{
    // Setup the layer
    map_bg = bgInit(bg_layer, BgType_Text8bpp, BgSize_T_512x256, map_base, tile_base);

    // Load tiles and palette, they won't change
    dmaCopy(tiles, bgGetGfxPtr(map_bg), tiles_size);
    dmaCopy(palette, BG_PALETTE, palette_size);

    // Keep some information about the map
    map_size_width = map_width;
    map_size_height = map_height;
    map_data = map;
}

int map_get_scroll_x(void)
{
    return map_current_scroll_x;
}

int map_get_scroll_y(void)
{
    return map_current_scroll_y;
}

void map_refresh_scroll(void)
{
    bgSetScroll(map_bg, map_get_scroll_x(), map_get_scroll_y());
}

int map_get_max_scroll_x(void)
{
    return map_size_width - SCREEN_WIDTH;
}

int map_get_max_scroll_y(void)
{
    return map_size_height - SCREEN_HEIGHT;
}

static inline u32 vram_map_entry_index(u32 tx, u32 ty)
{
    u32 sbb = tx / 32;
    return sbb * 1024 + ty * 32 + tx % 32;
}

static void map_load_row(int x, int y)
{
    const u16 *src = (const u16 *)map_data;
    u16 *dst = (u16 *)bgGetMapPtr(map_bg);

    u32 src_index = (y * (map_size_width / 8)) + x;

    for (int i = 0; i < (SCREEN_WIDTH / 8) + 1; i++)
    {
        u32 dst_index = vram_map_entry_index((x + i) & (64 - 1), y & (32 - 1));
        dst[dst_index] = src[src_index];
        src_index++;
    }
}

static void map_load_column(int x, int y)
{
    const u16 *src = (const u16 *)map_data;
    u16 *dst = (u16 *)bgGetMapPtr(map_bg);

    u32 src_index = (y * (map_size_width / 8)) + x;

    for (int j = 0; j < (SCREEN_HEIGHT / 8) + 1; j++)
    {
        u32 dst_index = vram_map_entry_index(x & (64 - 1), (y + j) & (32 - 1));
        dst[dst_index] = src[src_index];
        src_index += map_size_width / 8;
    }
}

void map_set_position(int x, int y)
{
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;

    const int max_scroll_x = map_get_max_scroll_x();
    const int max_scroll_y = map_get_max_scroll_y();

    if (x > max_scroll_x)
        x = max_scroll_x;
    if (y > max_scroll_y)
        y = max_scroll_y;

    map_current_scroll_x = x;
    map_current_scroll_y = y;

    const int tile_x = x / 8;
    const int tile_y = y / 8;

    for (int j = 0; j < (SCREEN_HEIGHT / 8) + 1; j++)
        map_load_row(tile_x, tile_y + j);
}

void map_scroll_x(int delta)
{
    while (delta > 0)
    {
        if (map_current_scroll_x >= map_get_max_scroll_x())
            return;

        if ((map_current_scroll_x & (8 - 1)) == 7)
        {
            map_load_column((map_current_scroll_x + SCREEN_WIDTH + 1) / 8,
                            map_current_scroll_y / 8);
        }

        map_current_scroll_x++;
        delta--;
    }

    while (delta < 0)
    {
        if (map_current_scroll_x <= 0)
            return;

        if ((map_current_scroll_x & (8 - 1)) == 0)
        {
            map_load_column((map_current_scroll_x - 1) / 8,
                            map_current_scroll_y / 8);
        }

        map_current_scroll_x--;
        delta++;
    }
}

void map_scroll_y(int delta)
{
    while (delta > 0)
    {
        if (map_current_scroll_y >= map_get_max_scroll_y())
            return;

        if ((map_current_scroll_y & (8 - 1)) == 7)
        {
            map_load_row(map_current_scroll_x / 8,
                         (map_current_scroll_y + SCREEN_HEIGHT + 1) / 8);
        }

        map_current_scroll_y++;
        delta--;
    }

    while (delta < 0)
    {
        if (map_current_scroll_y <= 0)
            return;

        if ((map_current_scroll_y & (8 - 1)) == 0)
        {
            map_load_row(map_current_scroll_x / 8,
                         (map_current_scroll_y - 1) / 8);
        }

        map_current_scroll_y--;
        delta++;
    }
}
