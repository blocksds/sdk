# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2024

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= tests_sys_stack_smash_protection
GAME_TITLE	:= Stack smash protection test
GAME_SUBTITLE	:= Tests: System

CFLAGS		:= -fstack-protector-strong
LIBS		:= -lnds9d

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
