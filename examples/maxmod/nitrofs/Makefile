# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023
# SPDX-FileContributor: Michele Di Giorgio, 2024

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= maxmod_nitrofs
GAME_TITLE	:= NitroFS example
GAME_SUBTITLE	:= Maxmod

# Source code paths

AUDIODIRS	:= audio
# When a directory is added to NITROFSDIR, the Makefile rules will save the
# soundbank to that directory instead of the build directory.
NITROFSDIR	:= nitrofs

# Libraries

LIBS		:= -lnds9 -lmm9
LIBDIRS		:= $(BLOCKSDS)/libs/maxmod

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
