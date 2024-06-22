// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#ifndef HELPERS_H__
#define HELPERS_H__

#include <nds.h>

// Wait until the user presses START, then exits the game to the loader.
__attribute__((noreturn)) void wait_forever(void);

#endif // HELPERS_H__
