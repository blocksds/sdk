# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= version_check_in_make
GAME_TITLE	:= Check version in make
GAME_SUBTITLE	:= Version

# Version check

BLOCKSDS_VERSION_FILE = $(BLOCKSDS)/libs/version/blocksds_version.make

ifeq ("$(wildcard $(BLOCKSDS_VERSION_FILE))","")
$(error BlocksDS version too old. Please update.)
endif

include $(BLOCKSDS_VERSION_FILE)

# You can experiment by changing this version number. If the number is too high
# the project won't be able to be built!
$(eval $(call error_if_blocksds_version_less_than, 1, 14, 2))

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
