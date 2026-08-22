#!/usr/bin/env python3

# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2024-2026

from architectds import *

arm9 = Arm9Binary(
    sourcedirs=['source'],
)
arm9.add_grit_shared('graphics', 'dir.grit', 'shared_data',
                     in_reference_tileset='city_tileset.png')
arm9.generate_elf()

nds = NdsRom(
    binaries=[arm9],
    game_title='8 bpp, preexisting tileset',
    game_subtitle='2D graphics: BGs',
)
nds.generate_nds()

nds.run_command_line_arguments()
