# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023

STRIP		:= -s
BINMODE		:= 755

RM		:= rm -rf
MAKE		:= make
INSTALL		:= install

.PHONY: bin2c clean dldipatch dlditool grit install mkfatimg mmutil ndstool teaktool

all: bin2c dldipatch dlditool grit mkfatimg mmutil ndstool teaktool

bin2c:
	$(MAKE) -C bin2c

grit:
	$(MAKE) -C grit

dlditool:
	$(MAKE) -C dlditool

dldipatch:
	$(MAKE) -C dldipatch

mkfatimg:
	$(MAKE) -C mkfatimg

mmutil:
	$(MAKE) -C mmutil

ndstool:
	$(MAKE) -C ndstool

teaktool:
	$(MAKE) -C teaktool

INSTALLDIR	?= /opt/blocksds/core/tools
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	@$(V)$(RM) $(INSTALLDIR_ABS)
	@$(V)$(INSTALL) -d $(INSTALLDIR_ABS) \
		      $(INSTALLDIR_ABS)/dldipatch
	$(INSTALL) $(STRIP) -m $(BINMODE) dldipatch/dldipatch $(INSTALLDIR_ABS)/dldipatch/
	$(MAKE) -C bin2c install INSTALLDIR=$(INSTALLDIR_ABS)/bin2c
	$(MAKE) -C dlditool install INSTALLDIR=$(INSTALLDIR_ABS)/dlditool
	$(MAKE) -C grit install INSTALLDIR=$(INSTALLDIR_ABS)/grit
	$(MAKE) -C mkfatimg install INSTALLDIR=$(INSTALLDIR_ABS)/mkfatimg
	$(MAKE) -C mmutil install INSTALLDIR=$(INSTALLDIR_ABS)/mmutil
	$(MAKE) -C ndstool install INSTALLDIR=$(INSTALLDIR_ABS)/ndstool
	$(MAKE) -C teaktool install INSTALLDIR=$(INSTALLDIR_ABS)/teaktool

clean:
	$(MAKE) -C bin2c clean
	$(MAKE) -C dldipatch clean
	$(MAKE) -C dlditool clean
	$(MAKE) -C grit clean
	$(MAKE) -C mkfatimg clean
	$(MAKE) -C mmutil clean
	$(MAKE) -C ndstool clean
	$(MAKE) -C teaktool clean
