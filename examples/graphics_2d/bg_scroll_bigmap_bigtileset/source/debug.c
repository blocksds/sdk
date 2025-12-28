// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

#include <stdbool.h>

static bool debug_enabled = false;

void debug_mode_enable(bool enable)
{
    debug_enabled = enable;
}

bool debug_mode_is_enabled(void)
{
    return debug_enabled;
}
