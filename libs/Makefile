# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

CP		:= cp
INSTALL		:= install
MAKE		:= make
RM		:= rm -rf
MKDIR		:= mkdir

.PHONY: all clean dswifi install libnds maxmod

all: dswifi libnds maxmod

# Build targets
# =============

dswifi: libnds
	+$(MAKE) -C dswifi -j`nproc`

libnds:
	+$(MAKE) -C libnds -j`nproc`

maxmod:
	+$(MAKE) -C maxmod ds -j`nproc`

# Install target
# ==============

INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "INSTALLDIR = '$(INSTALLDIR)' (must not be empty)"
	@test $(INSTALLDIR)
	$(INSTALL) -d $(INSTALLDIR_ABS)/libs \
		      $(INSTALLDIR_ABS)/libs/dswifi \
		      $(INSTALLDIR_ABS)/libs/libnds \
		      $(INSTALLDIR_ABS)/libs/maxmod
	$(CP) -r dswifi/include dswifi/lib $(INSTALLDIR_ABS)/libs/dswifi
	$(CP) -r libnds/include libnds/lib $(INSTALLDIR_ABS)/libs/libnds
	$(CP) -r maxmod/include maxmod/lib $(INSTALLDIR_ABS)/libs/maxmod

# Clean target
# ============

clean:
	+$(MAKE) -C dswifi clean
	+$(MAKE) -C libnds clean
	+$(MAKE) -C maxmod clean
