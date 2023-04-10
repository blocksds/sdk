# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

STRIP		:= -s
BINMODE		:= 755

RM		:= rm -rf
MAKE		:= make
INSTALL		:= install

.PHONY: bin2c clean dldipatch dlditool grit install mkfatimg mmutil ndstool

all: bin2c dldipatch dlditool grit mkfatimg mmutil ndstool

bin2c:
	$(MAKE) -C bin2c -j`nproc`

grit:
	$(MAKE) -C grit -j`nproc`

dlditool:
	$(MAKE) -C dlditool -j`nproc`

dldipatch:
	$(MAKE) -C dldipatch -j`nproc`

mkfatimg:
	$(MAKE) -C mkfatimg -j`nproc`

mmutil:
	$(MAKE) -C mmutil -j`nproc`

ndstool:
	$(MAKE) -C ndstool -j`nproc`

INSTALLDIR	?= /opt/blocksds/core/tools
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	@$(V)$(RM) $(INSTALLDIR_ABS)
	@$(V)$(INSTALL) -d $(INSTALLDIR_ABS) \
		      $(INSTALLDIR_ABS)/dldi \
		      $(INSTALLDIR_ABS)/dldipatch
	$(INSTALL) dldi/r4tfv2.dldi $(INSTALLDIR_ABS)/dldi/
	$(INSTALL) $(STRIP) -m $(BINMODE) dldipatch/dldipatch $(INSTALLDIR_ABS)/dldipatch/
	$(MAKE) -C bin2c install INSTALLDIR=$(INSTALLDIR_ABS)/bin2c
	$(MAKE) -C dlditool install INSTALLDIR=$(INSTALLDIR_ABS)/dlditool
	$(MAKE) -C grit install INSTALLDIR=$(INSTALLDIR_ABS)/grit
	$(MAKE) -C mkfatimg install INSTALLDIR=$(INSTALLDIR_ABS)/mkfatimg
	$(MAKE) -C mmutil install INSTALLDIR=$(INSTALLDIR_ABS)/mmutil
	$(MAKE) -C ndstool install INSTALLDIR=$(INSTALLDIR_ABS)/ndstool

clean:
	$(MAKE) -C bin2c clean
	$(MAKE) -C dldipatch clean
	$(MAKE) -C dlditool clean
	$(MAKE) -C grit clean
	$(MAKE) -C mkfatimg clean
	$(MAKE) -C mmutil clean
	$(MAKE) -C ndstool clean
