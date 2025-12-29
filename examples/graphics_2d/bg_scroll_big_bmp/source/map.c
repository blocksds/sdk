// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// We need to use a 512x256 map. When we scroll horizontally we modify one
// column of pixels. If the background is as wide as the screen (256 pixels) we
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

void map_load(int bg_layer, int map_base,
              const void *map, size_t map_width, size_t map_height)
{
    // Setup the layer
    map_bg = bgInit(bg_layer, BgType_Bmp16, BgSize_B16_512x256, map_base, 0);

    // Keep some information about the map
    map_size_width = map_width;
    map_size_height = map_height;
    map_data = map;

    // Enable wrapping. Bitmap backgrounds don't wrap around by default
    bgWrapOn(bg_layer);
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

static void map_load_row(int x, int y)
{
    if (y >= map_size_height)
        return;

    // NOTE: If this was an 8-bit bitmap we would have to handle this
    // differently because we can't write to VRAM in 8-bit units.

    const u16 *src = (const u16 *)map_data;
    u16 *dst = (u16 *)bgGetMapPtr(map_bg);

    u32 src_index = (y * map_size_width) + x;

    int x_start = x;
    int x_end = x_start + SCREEN_WIDTH + 1;

    if (x_end > map_size_width)
        x_end = map_size_width;

    for (int i = x_start; i < x_end; i++)
    {
        u32 dst_index = (i & (512 - 1))
                      + (y & (256 - 1)) * 512;
        dst[dst_index] = src[src_index];
        src_index++;
    }
}

static void map_load_column(int x, int y)
{
    if (x >= map_size_width)
        return;

    const u16 *src = (const u16 *)map_data;
    u16 *dst = (u16 *)bgGetMapPtr(map_bg);

    u32 src_index = (y * map_size_width) + x;

    int y_start = y;
    int y_end = y_start + SCREEN_HEIGHT + 1;

    if (y_end > map_size_height)
        y_end = map_size_height;

    for (int j = y_start; j < y_end; j++)
    {
        u32 dst_index = (x & (512 - 1))
                      + (j & (256 - 1)) * 512;
        dst[dst_index] = src[src_index];
        src_index += map_size_width;
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

    for (int j = 0; j < SCREEN_HEIGHT + 1; j++)
        map_load_row(x, y + j);
}

void map_scroll_x(int delta)
{
    while (delta > 0)
    {
        if (map_current_scroll_x >= map_get_max_scroll_x())
            return;

        map_load_column(map_current_scroll_x + SCREEN_WIDTH + 1,
                        map_current_scroll_y);

        map_current_scroll_x++;
        delta--;
    }

    while (delta < 0)
    {
        if (map_current_scroll_x <= 0)
            return;

        map_load_column(map_current_scroll_x - 1,
                        map_current_scroll_y);

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

        map_load_row(map_current_scroll_x,
                     map_current_scroll_y + SCREEN_HEIGHT + 1);

        map_current_scroll_y++;
        delta--;
    }

    while (delta < 0)
    {
        if (map_current_scroll_y <= 0)
            return;

        map_load_row(map_current_scroll_x,
                     map_current_scroll_y - 1);

        map_current_scroll_y--;
        delta++;
    }
}
