# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= dyn_libs_cpp_demo
GAME_TITLE	:= C++ demo
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

.PHONY: clean_test test

DSLTOOL		?= $(BLOCKSDS)/tools/dsltool/dsltool

TEST_ELF	:= lib_test/test.elf
TEST_DSL	:= nitrofs/dsl/test.dsl

test:
	make -C lib_test

$(TEST_DSL): test $(ELF)
	@$(MKDIR) -p $(@D)
	@$(RM) $(TEST_DSL)
	$(DSLTOOL) -i $(TEST_ELF) -o $(TEST_DSL) -m $(ELF)

$(ROM): $(TEST_DSL)

clean: clean_test

clean_test:
	make clean -C lib_test
