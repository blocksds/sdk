#!/usr/bin/env python3

# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2024

from architectds import *

nitrofs = NitroFS()
nitrofs.add_gl2d_sprite_set('graphics/ruins', 'graphics/ruins/ruins.grit')
nitrofs.generate_image()

arm9 = Arm9Binary(
    sourcedirs=['source'],
)
arm9.generate_elf()

nds = NdsRom(
    binaries=[arm9, nitrofs],
    game_title='GL2D: FS: Sprite sets',
)
nds.generate_nds()

nds.run_command_line_arguments()
