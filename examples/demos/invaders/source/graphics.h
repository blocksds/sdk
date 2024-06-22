// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#ifndef GRAPHICS_H__
#define GRAPHICS_H__

#include "definitions.h"

// This function loads all sprite textures from the filesystem.
void load_sprites(void);

// This loads the specified background from the filesystem and displays it on
// the screen.
void load_background(const char *path);


// Draw all the invaders of the provided array.
void draw_invaders(invader *invaders);

// Draw one ship at the specified coordinates.
void draw_ship(int x, int y);

// Draw all the bullets of the provided array.
void draw_bullets(bullet *bullets);


// Free all previously loaded graphics.
void free_graphics(void);

#endif // GRAPHICS_H__
