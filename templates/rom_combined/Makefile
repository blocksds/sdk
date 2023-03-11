# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

# User config
# ===========

NAME		:= test

NITRO_FILES	:=

GAME_TITLE	:= Test ROM
GAME_SUBTITLE1	:= Built with BlocksDS
GAME_SUBTITLE2	:= http://skylyrac.net
GAME_ICON	:= icon.bmp

# Root folder of the SD image and name for the generated image.
# Name it "DSi-1.sd" for no$gba in DSi mode
SDROOT		:= sdroot
SDIMAGE		:= image.bin

# Tools
# -----

MAKE		:= make
RM		:= rm -rf

# Verbose flag
# ------------

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

# Directories
# -----------

ARM9DIR		:= arm9
ARM7DIR		:= arm7

# Build artfacts
# --------------

ROM		:= $(NAME).nds

# Targets
# -------

.PHONY: all clean arm9 arm7 dldipatch sdimage

all: $(ROM)

clean:
	@echo "  CLEAN"
	$(V)$(MAKE) -f Makefile.arm9 clean --no-print-directory
	$(V)$(MAKE) -f Makefile.arm7 clean --no-print-directory
	$(V)$(RM) $(ROM) build $(SDIMAGE)

# Rules
# -----

arm9:
	$(V)+$(MAKE) -f Makefile.arm9 --no-print-directory

arm7:
	$(V)+$(MAKE) -f Makefile.arm7 --no-print-directory

ifneq ($(strip $(NITRO_FILES)),)
_NITRO_FILES	:=	-d $(NITRO_FILES)
endif

$(ROM): arm9 arm7
	@echo "  NDSTOOL $@"
	$(V)$(BLOCKSDS)/tools/ndstool/ndstool -c $@ \
		-7 build/arm7.elf -9 build/arm9.elf \
		-b $(GAME_ICON) "$(GAME_TITLE);$(GAME_SUBTITLE1);$(GAME_SUBTITLE2)" \
		$(_NITRO_FILES)

sdimage:
	@echo "  IMGBUILD $(SDIMAGE) $(SDROOT)"
	$(V)sh $(BLOCKSDS)/tools/imgbuild/imgbuild.sh $(SDIMAGE) $(SDROOT)

dldipatch: $(ROM)
	@echo "  DLDITOOL $(ROM)"
	$(V)$(BLOCKSDS)/tools/dlditool/dlditool \
		$(BLOCKSDS)/tools/dldi/r4tfv2.dldi $(ROM)
