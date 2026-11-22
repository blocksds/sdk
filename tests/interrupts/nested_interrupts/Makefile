# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2026

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= tests_irq_nested
GAME_TITLE	:= Nested interrupts
GAME_SUBTITLE	:= Tests: Interrupts

# Libraries

# Use the debug version of libnds to add extra checking to multithreading
LIBS := -lnds9d

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
