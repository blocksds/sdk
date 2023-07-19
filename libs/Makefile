# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

CP		:= cp
INSTALL		:= install
MAKE		:= make
RM		:= rm -rf
MKDIR		:= mkdir

.PHONY: all clean dswifi install libnds libxm7 maxmod

all: dswifi libnds libxm7 maxmod

# Build targets
# =============

dswifi: libnds
	+$(MAKE) -C dswifi

libnds:
	+$(MAKE) -C libnds

libxm7: libnds
	+$(MAKE) -C libxm7

maxmod:
	+$(MAKE) -C maxmod ds

# Install target
# ==============

INSTALLDIR	?= /opt/blocksds/core/libs
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	@$(RM) $(INSTALLDIR_ABS)
	@$(INSTALL) -d $(INSTALLDIR_ABS)
	+$(MAKE) -C libnds install INSTALLDIR=$(INSTALLDIR_ABS)/libnds
	+$(MAKE) -C dswifi install INSTALLDIR=$(INSTALLDIR_ABS)/dswifi
	+$(MAKE) -C libxm7 install INSTALLDIR=$(INSTALLDIR_ABS)/libxm7
	+$(MAKE) -C maxmod install INSTALLDIR=$(INSTALLDIR_ABS)/maxmod

# Clean target
# ============

clean:
	+$(MAKE) -C dswifi clean
	+$(MAKE) -C libnds clean
	+$(MAKE) -C libxm7 clean
	+$(MAKE) -C maxmod clean
