# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= tests_dtcm_fixed_size
GAME_TITLE	:= DTCM fixed size
GAME_SUBTITLE	:= Tests: System

LDFLAGS		:= -Wl,--defsym,__dtcm_data_size=16

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
