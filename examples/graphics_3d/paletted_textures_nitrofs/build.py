#!/usr/bin/env python3

# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2024

from architectds import *

nitrofs = NitroFS()
nitrofs.add_grit(['graphics'])
nitrofs.generate_image()

arm9 = Arm9Binary(
    sourcedirs=['source'],
)
arm9.generate_elf()

nds = NdsRom(
    binaries=[arm9, nitrofs],
    game_title='NitroFS paletted texture',
    game_subtitle='3D graphics',
)
nds.generate_nds()

nds.run_command_line_arguments()
