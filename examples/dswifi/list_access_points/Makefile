# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2024

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= dswifi_list_access_points
GAME_TITLE	:= List Access Points
GAME_SUBTITLE	:= DSWiFi

# Libraries
# ---------

LIBS		:= -ldswifi9 -lnds9
LIBDIRS		:= $(BLOCKSDS)/libs/dswifi \
			   $(BLOCKSDS)/libs/libnds

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
