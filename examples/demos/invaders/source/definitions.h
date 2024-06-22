// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#ifndef DEFINITIONS_H__
#define DEFINITIONS_H__

#include <stdbool.h>

// The defines below are the dimensions of the PNG files. They are used to
// calculate collisions and the initial placement of sprites on the screen.

#define BULLET_WIDTH    2
#define BULLET_HEIGHT   8

#define INVADER_WIDTH   22
#define INVADER_HEIGHT  17

#define SHIP_WIDTH      22
#define SHIP_HEIGHT     12

// Distribution of invaders on the screen

#define NUM_INVADERS_X  7
#define NUM_INVADERS_Y  4
#define NUM_INVADERS    (NUM_INVADERS_X * NUM_INVADERS_Y)

#define SPACE_BETWEEN_INVADERS_X 10
#define SPACE_BETWEEN_INVADERS_Y 10

// Maximum number of bullets allowed at the same time on the screen

#define NUM_BULLETS     1

// Speed of bullets in pixels per frame (floating point value)

#define BULLET_SPEED    1.5

// Internal state of invaders and bullets

typedef struct {
    bool active;
    float x, y;
    int anim_frame;
    int anim_count;
} invader;

typedef struct {
    bool active;
    float x, y;
} bullet;

#endif // DEFINITIONS_H__

