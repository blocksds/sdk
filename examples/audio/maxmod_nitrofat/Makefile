# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= maxmod_nitrofat
GAME_TITLE	:= Maxmod NitroFAT example

# Source code paths

SOURCEDIRS	:= source
AUDIODIRS	:= audio
# When a directory is added to NITROFATDIR, the Makefile rules will save the
# soundbank to that directory instead of the build directory.
NITROFATDIR	:= nitrofat

# Libraries

LIBS		:= -lnds9 -lmm9
LIBDIRS		:= $(BLOCKSDS)/libs/maxmod

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
