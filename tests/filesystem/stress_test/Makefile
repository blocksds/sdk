# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

# Having python3 as a hard dependency of BlocksDS just for this test is a bad
# idea. The Makefile of the parent folder will try to build this test because it
# iterates every child folder, so this Makefile simply skips the build if
# python3 isn't found.

ifeq (`which python3`,)

$(warning "Skipping filesystem stress test because python3 wasn't found")

else

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= filesystem_stress_test
GAME_TITLE	:= Filesystem stress test

# Source code paths

NITROFSDIR	:= nitrofs

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile

nitrofs:
	@echo "  GEN_FILESYSTEM"
	@python3 gen_filesystem.py

clean: clean_nitrofs

clean_nitrofs:
	@echo "  CLEAN NITROFS"
	@rm -rf nitrofs

endif
