# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

BLOCKSDS	?= /opt/blocksds/core
BLOCKSDSEXT	?= /opt/blocksds/external

# Tools
# -----

CP		:= cp
RM		:= rm -rf
INSTALL		:= install

# Verbose flag
# ------------

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

# Targets
# -------

.PHONY: all install

INSTALLDIR	?= /opt/blocksds/core/sys/default_makefiles
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

all:
	@echo "Nothing to build"

install:
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	$(V)$(RM) $(INSTALLDIR_ABS)
	$(V)$(INSTALL) -d $(INSTALLDIR_ABS)
	$(V)$(CP) -r rom_arm9 rom_arm9arm7 rom_arm9arm7teak ./COPYING* $(INSTALLDIR_ABS)
