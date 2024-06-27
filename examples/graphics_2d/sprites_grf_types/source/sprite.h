// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2024 Antonio Niño Díaz

#ifndef GRF_SPRITE_H__
#define GRF_SPRITE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <nds/arm9/sprite.h>

/// Loads a GRF file containing a sprite and its associated palette (if any).
///
/// This function works with 16-color, 256-color and 16 bpp bitmap sprites.
///
/// When extended palettes are enabled, it is possible to specify an extended
/// palette in palIndex. If extended palettes are disabled, or the sprite is a
/// bitmap sprite, the palette index must be zero.
///
/// This function is meant to be used before oamSet(). You should pass gfxOut,
/// sizeOut and formatOut to oamSet().
///
/// @param oam Must be &oamMain or &oamSub.
/// @param path Path to the GRF file in the filesystem.
/// @param palIndex Palette index (16 color palette or 256 color ext. palette).
/// @param gfxOut Pointer to be filled with the address of the loaded graphics.
/// @param sizeOut Pointer to be filled with the size of the sprite.
/// @param formatOut Pointer to be filled with the format of the sprite.
/// @return It returns zero on success, an error code on error.
int oamLoadGfxGrf(OamState *oam, const char *path, int palIndex, void **gfxOut,
                  SpriteSize *sizeOut, SpriteColorFormat *formatOut);

#ifdef __cplusplus
}
#endif

#endif // GRF_SPRITE_H__
