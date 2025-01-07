# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2024

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= libxm7_basic_sound

GAME_TITLE	:= Basic example
GAME_SUBTITLE	:= LibXM7
GAME_AUTHOR	:= (sverx, 2009-01-07)

# Source code paths

BINDIRS		:= data

# Libraries

ARM7ELF		:= $(BLOCKSDS)/sys/arm7/main_core/arm7_dswifi_libxm7.elf

LIBS		:= -lnds9 -lxm79
LIBDIRS		:= $(BLOCKSDS)/libs/libxm7 \
		   $(BLOCKSDS)/libs/libnds

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
