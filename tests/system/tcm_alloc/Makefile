# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= tests_tcm_alloc
GAME_TITLE	:= TCM allocation
GAME_SUBTITLE	:= Tests: System

# Reserve some user-reserved DTCM space. dtcm_malloc() will use the space
# remaining after all static DTCM variables are allocated.
LDFLAGS		:= -Wl,--defsym,__dtcm_data_size=10000

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
