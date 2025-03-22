#!/usr/bin/env python3

# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

from architectds import *

arm9 = Arm9Binary(
    sourcedirs=['source'],
)
arm9.generate_elf()

# This library requires functions from the main binary
calculator = Arm9DynamicLibrary(
    name='calculator',
    main_binary=arm9,
    sourcedirs=['lib_calculator/source'],
)
calculator.generate_dsl()

# This library doesn't require functions from the main binary
cypher = Arm9DynamicLibrary(
    name='cypher',
    sourcedirs=['lib_cypher/source'],
)
cypher.generate_dsl()

nitrofs = NitroFS()
nitrofs.add_arm9_dsl(calculator)
nitrofs.add_arm9_dsl(cypher)
nitrofs.generate_image()

nds = NdsRom(
    binaries=[arm9, calculator, cypher, nitrofs],
    nds_path='dyn_libs_two_libraries.nds',
    game_title='Two dynamic libraries',
    game_subtitle='Dynamic libraries',
)
nds.generate_nds()

nds.run_command_line_arguments()
