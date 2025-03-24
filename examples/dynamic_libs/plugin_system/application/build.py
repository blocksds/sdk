#!/usr/bin/env python3

# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

from architectds import *

arm9 = Arm9Binary(
    sourcedirs=['source'],
)
arm9.generate_elf()

nds = NdsRom(
    binaries=[arm9],
    nitrofsdirs=['nitrofs'],
    nds_path='dyn_libs_plugin_system.nds',
    game_title='Plugin system',
    game_subtitle='Dynamic libraries',
)
nds.generate_nds()

nds.run_command_line_arguments()
