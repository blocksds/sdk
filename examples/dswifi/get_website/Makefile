# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2024

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= dswifi_get_website
GAME_TITLE	:= Get website
GAME_SUBTITLE	:= DSWiFi

# Libraries
# ---------

LIBS		:= -ldswifi9 -lnds9
LIBDIRS		:= $(BLOCKSDS)/libs/dswifi \
			   $(BLOCKSDS)/libs/libnds

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
