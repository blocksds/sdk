# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2023-2025

# Tools
# -----

STRIP		:= -s
BINMODE		:= 755

RM		:= rm -rf
MAKE		:= make
INSTALL		:= install

# Version string handling
# -----------------------

# Try to generate a version string if it isn't already provided
ifeq ($(VERSION_STRING),)
    # Try an exact match with a tag (e.g. v1.12.1)
    VERSION_STRING	:= $(shell git describe --tags --exact-match --dirty 2>/dev/null)
    ifeq ($(VERSION_STRING),)
        # Try a non-exact match (e.g. v1.12.1-3-g67a811a)
        VERSION_STRING	:= $(shell git describe --tags --dirty 2>/dev/null)
        ifeq ($(VERSION_STRING),)
            # If no version is provided by the user or git, fall back to this
            VERSION_STRING	:= DEV
        endif
    endif
endif

# Verbose flag
# ------------

ifeq ($(VERBOSE),1)
V		:=
else
V		:= @
endif

# Targets
# -------

.PHONY: bin2c clean dldipatch dlditool dsltool grit install mkfatimg mmutil \
	ndstool squeezer teaktool

all: bin2c dldipatch dlditool dsltool grit mkfatimg mmutil ndstool squeezer \
	teaktool

bin2c:
	$(MAKE) -C bin2c VERSION_STRING=$(VERSION_STRING)

grit:
	$(MAKE) -C grit VERSION_STRING=$(VERSION_STRING)

dlditool:
	$(MAKE) -C dlditool VERSION_STRING=$(VERSION_STRING)

dldipatch:
	$(MAKE) -C dldipatch VERSION_STRING=$(VERSION_STRING)

dsltool:
	$(MAKE) -C dsltool VERSION_STRING=$(VERSION_STRING)

mkfatimg:
	$(MAKE) -C mkfatimg VERSION_STRING=$(VERSION_STRING)

mmutil:
	$(MAKE) -C mmutil VERSION_STRING=$(VERSION_STRING)

ndstool:
	$(MAKE) -C ndstool VERSION_STRING=$(VERSION_STRING)

squeezer:
	$(MAKE) -C squeezer VERSION_STRING=$(VERSION_STRING)

teaktool:
	$(MAKE) -C teaktool VERSION_STRING=$(VERSION_STRING)

INSTALLDIR	?= /opt/blocksds/core/tools
INSTALLDIR_ABS	:= $(abspath $(INSTALLDIR))

install: all
	@echo "  INSTALL $(INSTALLDIR_ABS)"
	@test $(INSTALLDIR_ABS)
	@$(V)$(RM) $(INSTALLDIR_ABS)
	$(MAKE) -C bin2c install INSTALLDIR=$(INSTALLDIR_ABS)/bin2c
	$(MAKE) -C dldipatch install INSTALLDIR=$(INSTALLDIR_ABS)/dldipatch
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
