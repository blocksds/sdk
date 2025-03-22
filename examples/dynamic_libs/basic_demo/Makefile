# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= dyn_libs_basic_demo
GAME_TITLE	:= Basic demo
GAME_SUBTITLE	:= Dynamic libraries

# Source code paths

INCLUDEDIRS	:= source
NITROFSDIR	:= nitrofs

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile

# Additional rules to build the dynamic library:
#
# 1. Build ELF file of main ARM9 binary.
# 2. Build ELF file of library.
# 3. Generate DSL of the library taking both ELF files as input.
# 4. Save the DSL in the NitroFS folder.
# 5. Build NDS ROM.

.PHONY: clean_calculator calculator

DSLTOOL		?= $(BLOCKSDS)/tools/dsltool/dsltool

CALCULATOR_ELF	:= lib_calculator/calculator.elf
CALCULATOR_DSL	:= nitrofs/dsl/calculator.dsl

calculator:
	make -C lib_calculator

$(CALCULATOR_DSL): calculator $(ELF)
	@$(MKDIR) -p $(@D)
	@$(RM) $(CALCULATOR_DSL)
	$(DSLTOOL) -i $(CALCULATOR_ELF) -o $(CALCULATOR_DSL) -m $(ELF)

$(ROM): $(CALCULATOR_DSL)

clean: clean_calculator

clean_calculator:
	make clean -C lib_calculator
