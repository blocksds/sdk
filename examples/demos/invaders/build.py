#!/usr/bin/env python3

# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2024

from architectds import *

nitrofs = NitroFS()
nitrofs.add_grit(['graphics/backgrounds'], out_dir='graphics/backgrounds')
nitrofs.add_grit(['graphics/sprites'], out_dir='grit/sprites')
nitrofs.add_gl2d_sprite_set('graphics/invaders', 'graphics/invaders/invaders.grit')
nitrofs.generate_image()

arm9 = Arm9Binary(
    sourcedirs=['source'],
)
arm9.generate_elf()

nds = NdsRom(
    binaries=[arm9, nitrofs],
    game_title='Invaders',
    game_subtitle='Demo',
)
nds.generate_nds()

nds.run_command_line_arguments()
