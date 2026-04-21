# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

BLOCKSDS	?= /opt/blocksds/core
BLOCKSDSEXT	?= /opt/blocksds/external
WONDERFUL_TOOLCHAIN	?= /opt/wonderful

# User config
# ===========

NAME		?= template

GAME_TITLE	?= Template
GAME_SUBTITLE	?= Built with BlocksDS
GAME_AUTHOR	?= blocksds.skylyrac.net
GAME_ICON	?= $(BLOCKSDS)/sys/icon.gif

# A compile_commands.json file is created if this is set to 1
COMPDB		?= 0

# DLDI and internal SD slot of DSi
# --------------------------------

# Root folder of the SD image
SDROOT		?= sdroot
# Name of the generated image it "DSi-1.sd" for no$gba in DSi mode
SDIMAGE		?= image.bin

# Source code paths
# -----------------

# List of folders to combine into the root of NitroFS:
NITROFSDIR	?=

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
	$(V)$(RM) $(ROM) build $(SDIMAGE) compile_commands.json

arm9:
	$(V)+$(MAKE) -f Makefile.arm9 COMPDB=$(COMPDB) --no-print-directory

arm7:
	$(V)+$(MAKE) -f Makefile.arm7 COMPDB=$(COMPDB) --no-print-directory

ifeq ($(COMPDB),1)
# Add an additional dependency to the "all" rule
all: compile_commands.json

compile_commands.json: arm9 arm7
	@echo "  MERGE   compile_commands.json"
	$(V)$(WONDERFUL_TOOLCHAIN)/bin/wf-compile-commands-merge $@ \
		build/*/compile_commands.json
endif

ifneq ($(strip $(NITROFSDIR)),)
# Additional arguments for ndstool
NDSTOOL_ARGS	:= -d $(NITROFSDIR)

# Make the NDS ROM depend on the filesystem only if it is needed
$(ROM): $(NITROFSDIR)
endif

# Combine the title strings
ifeq ($(strip $(GAME_SUBTITLE)),)
    GAME_FULL_TITLE := $(GAME_TITLE);$(GAME_AUTHOR)
else
    GAME_FULL_TITLE := $(GAME_TITLE);$(GAME_SUBTITLE);$(GAME_AUTHOR)
endif

$(ROM): arm9 arm7
	@echo "  NDSTOOL $@"
	$(V)$(BLOCKSDS)/tools/ndstool/ndstool -c $@ \
		-7 build/arm7.elf -9 build/arm9.elf \
		-b $(GAME_ICON) "$(GAME_FULL_TITLE)" \
		$(NDSTOOL_ARGS)

sdimage:
	@echo "  MKFATIMG $(SDIMAGE) $(SDROOT)"
	$(V)$(BLOCKSDS)/tools/mkfatimg/mkfatimg -t $(SDROOT) $(SDIMAGE)

dldipatch: $(ROM)
	@echo "  DLDIPATCH $(ROM)"
	$(V)$(BLOCKSDS)/tools/dldipatch/dldipatch patch \
		$(BLOCKSDS)/sys/dldi_r4/r4tf.dldi $(ROM)
