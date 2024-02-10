#!/usr/bin/env python3

# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2024

from architectds import *

teak1 = TeakBinary(
    name='teak1',
    sourcedirs=['teak/source1'],
)
teak1.generate_tlf()

teak2 = TeakBinary(
    name='teak2',
    sourcedirs=['teak/source2'],
)
teak2.generate_tlf()

nitrofs = NitroFS()
nitrofs.add_tlf(teak2)
nitrofs.generate_image()

arm9 = Arm9Binary(
    sourcedirs=['arm9/source'],
)
arm9.add_tlf(teak1)
arm9.generate_elf()

nds = NdsRom(
    binaries=[teak1, teak2, arm9, nitrofs],
    game_title='Multiple DSP binaries',
)
nds.generate_nds()

nds.run_command_line_arguments()
