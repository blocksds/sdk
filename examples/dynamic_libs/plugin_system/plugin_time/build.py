#!/usr/bin/env python3

# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

from architectds import *

# This library doesn't require functions from the main binary
plugin = Arm9DynamicLibrary(
    name='time',
    sourcedirs=['source'],
)
plugin.generate_dsl_standalone()

plugin.run_command_line_arguments()
