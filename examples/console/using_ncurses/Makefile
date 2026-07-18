# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

BLOCKSDS	?= /opt/blocksds/core
BLOCKSDSEXT	?= /opt/blocksds/external

# Only build this example if ncurses is found
NCURSES_FOUND	:= $(shell find $(BLOCKSDSEXT)/ncurses -name libncurses.a 2> /dev/null)

ifneq ($(strip $(NCURSES_FOUND)),)
    PACKAGES_FOUND	:= true
endif

ifneq ($(strip $(PACKAGES_FOUND)),true)

.PHONY: all clean

all:
	@echo "Error: Packages missing. Run this command:"
	@echo "    wf-pacman -Sy blocksds-ncurses"

clean:
	@echo "  CLEAN"

else

# User config

NAME		:= console_ncurses

ifeq ($(DEBUG),1)
NAME	:= $(NAME)_debug
endif

GAME_TITLE	:= Using ncurses
GAME_SUBTITLE	:= Console

# Source code paths

GFXDIRS		:= graphics
NITROFSDIR	:= nitrofs

# Libraries

ifeq ($(DEBUG),1)
    LIBS    := -lnds9d -lncurses_g
    DEFINES := -DDEBUG
else
    LIBS    := -lnds9 -lncurses
endif

LIBDIRS := $(BLOCKSDS)/libs/libnds \
		   $(BLOCKSDSEXT)/ncurses

INCLUDEDIRS	:= $(BLOCKSDSEXT)/ncurses/include/ncurses

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile

endif
