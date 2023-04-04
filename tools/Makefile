# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

STRIP		:= -s
BINMODE		:= 755

MAKE		:= make
INSTALL		:= install

.PHONY: bin2c clean dldipatch dlditool grit install mkfatimg mmutil ndstool

all: bin2c dldipatch dlditool grit mkfatimg mmutil ndstool

bin2c: bin2c/bin2c

bin2c/bin2c:
	$(MAKE) -C bin2c -j`nproc`

grit: grit/grit

grit/grit:
	$(MAKE) -C grit -j`nproc`

dlditool: dlditool/dlditool

dlditool/dlditool:
	$(MAKE) -C dlditool -j`nproc`

dldipatch: dldipatch/dldipatch

dldipatch/dldipatch:
	$(MAKE) -C dldipatch -j`nproc`

mkfatimg: mkfatimg/mkfatimg

mkfatimg/mkfatimg:
	$(MAKE) -C mkfatimg -j`nproc`

mmutil: mmutil/mmutil

mmutil/mmutil:
	$(MAKE) -C mmutil -j`nproc`

ndstool: ndstool/ndstool

ndstool/ndstool:
	$(MAKE) -C ndstool -j`nproc`

INSTALLDIR	?= /opt/blocksds/core/tools
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	@$(V)$(RM) $(INSTALLDIR_ABS)
	@$(V)$(INSTALL) -d $(INSTALLDIR_ABS) \
		      $(INSTALLDIR_ABS)/bin2c \
		      $(INSTALLDIR_ABS)/dldi \
		      $(INSTALLDIR_ABS)/dldipatch \
		      $(INSTALLDIR_ABS)/dlditool \
		      $(INSTALLDIR_ABS)/grit \
		      $(INSTALLDIR_ABS)/mkfatimg \
		      $(INSTALLDIR_ABS)/mmutil \
		      $(INSTALLDIR_ABS)/ndstool
	$(INSTALL) $(STRIP) -m $(BINMODE) bin2c/bin2c $(INSTALLDIR_ABS)/bin2c/
	$(INSTALL) dldi/r4tfv2.dldi $(INSTALLDIR_ABS)/dldi/
	$(INSTALL) $(STRIP) -m $(BINMODE) dldipatch/dldipatch $(INSTALLDIR_ABS)/dldipatch/
	$(INSTALL) $(STRIP) -m $(BINMODE) dlditool/dlditool $(INSTALLDIR_ABS)/dlditool/
	$(INSTALL) $(STRIP) -m $(BINMODE) grit/grit $(INSTALLDIR_ABS)/grit/
	$(INSTALL) $(STRIP) -m $(BINMODE) mkfatimg/mkfatimg $(INSTALLDIR_ABS)/mkfatimg/
	$(INSTALL) $(STRIP) -m $(BINMODE) mmutil/mmutil $(INSTALLDIR_ABS)/mmutil/
	$(INSTALL) $(STRIP) -m $(BINMODE) ndstool/ndstool $(INSTALLDIR_ABS)/ndstool/

clean:
	$(MAKE) -C bin2c clean
	$(MAKE) -C dldipatch clean
	$(MAKE) -C dlditool clean
	$(MAKE) -C grit clean
	$(MAKE) -C mkfatimg clean
	$(MAKE) -C mmutil clean
	$(MAKE) -C ndstool clean
