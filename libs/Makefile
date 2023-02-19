# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

CP		:= cp
INSTALL		:= install
MAKE		:= make

.PHONY: all clean dswifi install libnds libsysnds maxmod

all: dswifi libnds libsysnds maxmod

dswifi: libnds
	+$(MAKE) -C dswifi -f Makefile.blocks -j`nproc`

libnds:
	+$(MAKE) -C libnds -f Makefile.blocks -j`nproc`

libsysnds:
	+$(MAKE) -C libsysnds -j`nproc`

maxmod:
	+$(MAKE) -C maxmod -f Makefile.blocks ds -j`nproc`

INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "INSTALLDIR = '$(INSTALLDIR)' (must not be empty)"
	@test $(INSTALLDIR)
	$(INSTALL) -d $(INSTALLDIR_ABS)/libs \
		      $(INSTALLDIR_ABS)/libs/dswifi \
		      $(INSTALLDIR_ABS)/libs/libnds \
		      $(INSTALLDIR_ABS)/libs/libsysnds \
		      $(INSTALLDIR_ABS)/libs/maxmod
	$(CP) -r dswifi/include dswifi/lib $(INSTALLDIR_ABS)/libs/dswifi
	$(CP) -r libnds/include libnds/lib $(INSTALLDIR_ABS)/libs/libnds
	$(CP) -r libsysnds/lib $(INSTALLDIR_ABS)/libs/libsysnds
	$(CP) -r maxmod/include maxmod/lib $(INSTALLDIR_ABS)/libs/maxmod

clean:
	+$(MAKE) -C dswifi -f Makefile.blocks clean
	+$(MAKE) -C libnds -f Makefile.blocks clean
	+$(MAKE) -C libsysnds clean
	+$(MAKE) -C maxmod -f Makefile.blocks clean
