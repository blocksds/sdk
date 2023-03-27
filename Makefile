# SPDX-License-Identifier: Zlib
#
# Copyright (c) 2023 Antonio Niño Díaz

# Default installation path of BlocksDS core components
INSTALLDIR	?= /opt/blocksds/core/

.PHONY: all clean examples install libs sys templates tests tools

all: libs sys tools

MAKE		:= make

examples:
	+$(MAKE) -C examples

# libnds depends on grit and bin2c
libs: tools
	+$(MAKE) -C libs

# The default ARM7 depends on some libraries
sys: libs
	+$(MAKE) -C sys

templates:
	+$(MAKE) -C templates

tests:
	+$(MAKE) -C tests

tools:
	+$(MAKE) -C tools

install: all
	@echo "INSTALLDIR = '$(INSTALLDIR)' (must not be empty)"
	@test $(INSTALLDIR)
	@rm -rf $(INSTALLDIR)
	+$(MAKE) -C libs install INSTALLDIR=$(abspath $(INSTALLDIR))
	+$(MAKE) -C sys install INSTALLDIR=$(abspath $(INSTALLDIR))
	+$(MAKE) -C tools install INSTALLDIR=$(abspath $(INSTALLDIR))
	@if [ `git rev-parse HEAD 2> /dev/null` ]; then \
		git rev-parse HEAD > $(INSTALLDIR)/version.txt ; \
	fi

clean:
	+$(MAKE) -C examples clean
	+$(MAKE) -C libs clean
	+$(MAKE) -C sys clean
	+$(MAKE) -C tests clean
	+$(MAKE) -C templates clean
	+$(MAKE) -C tools clean
