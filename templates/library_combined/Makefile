# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2026

BLOCKSDS	?= /opt/blocksds/core
BLOCKSDSEXT	?= /opt/blocksds/external
WONDERFUL_TOOLCHAIN	?= /opt/wonderful

# User config
# ===========

# A compile_commands.json file is created if this is set to 1
COMPDB		?= 0

# Build artifacts
# ---------------

INSTALLNAME	:= library_combined

# Tools
# -----

MAKE		:= make
RM		:= rm -rf
CP		:= cp
INSTALL	:= install

# Verbose flag
# ------------

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

# Targets
# -------

.PHONY: all arm7 arm9 clean install

all: arm9 arm7

arm9:
	@+$(MAKE) -f Makefile.arm9 COMPDB=$(COMPDB) --no-print-directory

arm7:
	@+$(MAKE) -f Makefile.arm7 COMPDB=$(COMPDB) --no-print-directory

clean:
	@echo "  CLEAN"
	@$(RM) $(VERSION_HEADER) lib build compile_commands.json

install: all
	@echo "  INSTALL $(BLOCKSDSEXT)/$(INSTALLNAME)/"
	$(V)$(RM) $(BLOCKSDSEXT)/$(INSTALLNAME)/
	$(V)$(INSTALL) -d $(BLOCKSDSEXT)/$(INSTALLNAME)/
	$(V)$(CP) -r include lib $(BLOCKSDSEXT)/$(INSTALLNAME)/

ifeq ($(COMPDB),1)
# Add an additional dependency to the "all" rule
all: compile_commands.json

compile_commands.json: arm9 arm7
	@echo "  MERGE   compile_commands.json"
	$(V)$(WONDERFUL_TOOLCHAIN)/bin/wf-compile-commands-merge $@ \
		build/*/compile_commands.json
endif
