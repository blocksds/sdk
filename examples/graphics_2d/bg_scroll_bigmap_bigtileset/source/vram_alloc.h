// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#ifndef EXAMPLE_VRAM_ALLOC_H__
#define EXAMPLE_VRAM_ALLOC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <nds/ndstypes.h>

void vram_tiles_reset(void);

void vram_tile_set_used(u32 index);
void vram_tile_set_free(u32 index);

int vram_tile_find_free(void);

int vram_tiles_get_count_used(void);
int vram_tiles_get_count_max(void);

#ifdef __cplusplus
}
#endif

#endif // EXAMPLE_VRAM_ALLOC_H__
