# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2024

STRIP		:= -s
BINMODE		:= 755

RM		:= rm -rf
MAKE		:= make
INSTALL		:= install

# Verbose flag
# ------------

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

.PHONY: bin2c clean dldipatch dlditool dsltool grit install mkfatimg mmutil \
	ndstool squeezer teaktool

all: bin2c dldipatch dlditool dsltool grit mkfatimg mmutil ndstool squeezer \
	teaktool

bin2c:
	$(MAKE) -C bin2c SDK_VERSION=$(SDK_VERSION)

grit:
	$(MAKE) -C grit SDK_VERSION=$(SDK_VERSION)

dlditool:
	$(MAKE) -C dlditool

dldipatch:
	$(MAKE) -C dldipatch

dsltool:
	$(MAKE) -C dsltool

mkfatimg:
	$(MAKE) -C mkfatimg

mmutil:
	$(MAKE) -C mmutil SDK_VERSION=$(SDK_VERSION)

ndstool:
	$(MAKE) -C ndstool SDK_VERSION=$(SDK_VERSION)

squeezer:
	$(MAKE) -C squeezer

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
	$(MAKE) -C dsltool install INSTALLDIR=$(INSTALLDIR_ABS)/dsltool
	$(MAKE) -C grit install INSTALLDIR=$(INSTALLDIR_ABS)/grit
	$(MAKE) -C mkfatimg install INSTALLDIR=$(INSTALLDIR_ABS)/mkfatimg
	$(MAKE) -C mmutil install INSTALLDIR=$(INSTALLDIR_ABS)/mmutil
	$(MAKE) -C ndstool install INSTALLDIR=$(INSTALLDIR_ABS)/ndstool
	$(MAKE) -C squeezer install INSTALLDIR=$(INSTALLDIR_ABS)/squeezer
	$(MAKE) -C teaktool install INSTALLDIR=$(INSTALLDIR_ABS)/teaktool

clean:
	$(MAKE) -C bin2c clean
	$(MAKE) -C dldipatch clean
	$(MAKE) -C dlditool clean
	$(MAKE) -C dsltool clean
	$(MAKE) -C grit clean
	$(MAKE) -C mkfatimg clean
	$(MAKE) -C mmutil clean
	$(MAKE) -C ndstool clean
	$(MAKE) -C squeezer clean
	$(MAKE) -C teaktool clean
