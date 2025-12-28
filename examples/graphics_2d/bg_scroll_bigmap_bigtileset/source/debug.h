// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#ifndef EXAMPLE_DEBUG_H__
#define EXAMPLE_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

void debug_mode_enable(bool enable);
bool debug_mode_is_enabled(void);

#ifdef __cplusplus
}
#endif

#endif // EXAMPLE_DEBUG_H__
