# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

# User config
# ===========

NAME		:= directory

GAME_TITLE	:= Directories example
GAME_SUBTITLE	:= Built with BlocksDS
GAME_AUTHOR	:= github.com/blocksds/sdk
GAME_ICON	:= icon.bmp

# DLDI and internal SD slot of DSi
# --------------------------------

# Root folder of the SD image
SDROOT		:= sdroot
# Name of the generated image it "DSi-1.sd" for no$gba in DSi mode
SDIMAGE		:= image.bin

# Source code paths
# -----------------

SOURCEDIRS	:= source
INCLUDEDIRS	:=
GFXDIRS		:=
BINDIRS		:=
AUDIODIRS	:=
NITROFATDIR	:=

# Defines passed to all files
# ---------------------------

DEFINES		:=

# Libraries
# ---------

LIBS		:= -lmm9 -lnds9 -lc
LIBDIRS		:= $(BLOCKSDS)/libs/maxmod \
		   $(BLOCKSDS)/libs/libnds \
		   $(BLOCKSDS)/libs/libc9

include ../../Makefile.include
