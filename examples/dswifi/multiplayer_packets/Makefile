# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2024-2025

BLOCKSDS	?= /opt/blocksds/core

# User config

ifeq ($(DEBUG),1)
    NAME	:= dswifi_multiplayer_packets_debug
else
    NAME	:= dswifi_multiplayer_packets
endif

GAME_TITLE	:= Multiplayer packets
GAME_SUBTITLE	:= DSWiFi

# Libraries
# ---------

ifeq ($(DEBUG),1)
    DEFINES := -DSGIP_DEBUG -DDSWIFI_LOGS
    ARM7ELF	:= $(BLOCKSDS)/sys/arm7/main_core/arm7_dswifi_debug.elf
    LIBS	:= -ldswifi9d -lnds9d
else
    ARM7ELF	:= $(BLOCKSDS)/sys/arm7/main_core/arm7_dswifi.elf
    LIBS	:= -ldswifi9 -lnds9
endif

LIBDIRS		:= $(BLOCKSDS)/libs/dswifi \
			   $(BLOCKSDS)/libs/libnds

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
