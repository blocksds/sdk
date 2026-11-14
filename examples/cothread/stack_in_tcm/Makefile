# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= cothread_stack_in_tcm
GAME_TITLE	:= Stack in TCM
GAME_SUBTITLE	:= Cothread

# Libraries

LIBS		:= -lnds9d

# Compiler options

# Reserve space in DTCM to allocate threads
LDFLAGS		:= -Wl,--defsym,__dtcm_data_size=14000

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
