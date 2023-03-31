# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

BLOCKSDS	?= /opt/blocksds/core
BLOCKSDSEXT	?= /opt/blocksds/external

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
	@+$(MAKE) -f Makefile.arm9 --no-print-directory

arm7:
	@+$(MAKE) -f Makefile.arm7 --no-print-directory

clean:
	@echo "  CLEAN"
	@$(RM) $(VERSION_HEADER) lib build

install: all
	@echo "  INSTALL $(BLOCKSDSEXT)/$(INSTALLNAME)/"
	$(V)$(RM) $(BLOCKSDSEXT)/$(INSTALLNAME)/
	$(V)$(INSTALL) -d $(BLOCKSDSEXT)/$(INSTALLNAME)/
	$(V)$(CP) -r include lib $(BLOCKSDSEXT)/$(INSTALLNAME)/
